open ReactNative;

module Style = BsReactNative.Style;

open Utils;

module Styles = {
  open Style;
  let fill =
    style([
      position(Absolute),
      top(Pt(0.0)),
      left(Pt(0.0)),
      right(Pt(0.0)),
      bottom(Pt(0.0)),
    ]);
  let flex = style([flex(1.0)]);
  let card =
    style([
      backgroundColor(String("#E9E9EF")),
      shadowColor(String("black")),
      shadowOffset(~width=0.0, ~height=0.0),
      shadowOpacity(0.2),
      shadowRadius(5.0),
    ]);
  let stackContainer = list([flex, style([flexDirection(ColumnReverse)])]);
};

type commonNavigation('route, 'options) = {
  push: 'route => unit,
  replace: 'route => unit,
  setOptions: 'options => unit,
  pop: unit => unit,
};

module CreateStackNavigator = (Config: {type route;}) => {
  module StackNavigator = {
    module Animation = Animation;
    type screenConfig = {
      route: Config.route,
      key: string,
      header: Header.config,
      animatedValue: BsReactNative.Animated.Value.t,
      animation: Animation.t,
      didMount: bool,
      style: Style.t,
    };
    type headerMode =
      | Screen
      | Floating;
    type pendingTransition = (int, int);
    type state = {
      screens: array(screenConfig),
      pendingTransition: option(pendingTransition),
      headerAnimatedValue: BsReactNative.Animated.Value.t,
      activeScreen: int,
    };
    type persistedState = array(Config.route);
    type options = {
      header: Header.config,
      animation: option(Animation.t),
      style: option(Style.t),
    };
    type action =
      | PushScreen(Config.route, string)
      | ReplaceScreen(Config.route, string)
      | StartTransition([ | `Pop | `Replace | `Push], int, int)
      | SetOptions(options, string)
      | RemoveStaleScreen(string)
      | ReplaceScreenFinish(string)
      | RemoveLastScreen
      | PopScreen(string);
    type navigation = commonNavigation(Config.route, options);
    include Persistence.CreatePersistence({
      type state = persistedState;
    });
    let getNavigationInterface = (send, screenKey) => {
      push: route => send(PushScreen(route, screenKey)),
      replace: route => send(ReplaceScreen(route, screenKey)),
      pop: () => send(PopScreen(screenKey)),
      setOptions: opts => send(SetOptions(opts, screenKey)),
    };
    /**
     * Gestures
     */
    module Gestures = {
      /** PanGestureHandler from `react-native-gesture-handler */
      module PanHandler = {
        [@bs.module "react-native-gesture-handler"] [@react.component]
        external make:
          (
            ~onGestureEvent: 'onGestureEvent,
            ~maxDeltaX: 'maxDeltaX,
            ~minDeltaX: 'minDeltaX,
            ~enabled: 'enabled,
            ~onHandlerStateChange: 'onHandlerStateChange,
            ~hitSlop: 'hitSlop,
            ~children: React.element
          ) =>
          React.element =
          "PanGestureHandler";
      };
      let screenWidth = Dimensions.get(`window)##width;
      let animatedValue = BsReactNative.Animated.Value.create(0.0);
      let handler =
        BsReactNative.Animated.event(
          [|{
              "nativeEvent": {
                "translationX": animatedValue,
              },
            }|],
          {"useNativeDriver": true},
        );
      /** Interpolated progress in range of 0 to 1 (start to end) */
      let animatedProgress =
        BsReactNative.Animated.Value.interpolate(
          animatedValue,
          ~inputRange=[0.0, screenWidth],
          ~outputRange=`float([0.0, 1.0]),
          ~extrapolate=BsReactNative.Animated.Interpolation.Clamp,
          (),
        );
      /**
       * Called when gesture state changes (5 - end)
       *
       * At the end of the animation, make sure to reset gesture
       * state to `0` and update all the other animated values
       * accordingly.
       */
      let onStateChange = (event, self) => {
        let e = event##nativeEvent;
        switch (e##state) {
        | 5 =>
          let toValue =
            e##translationX > screenWidth /. 2. || e##velocityX > 150.00
              ? screenWidth : 0.;

          BsReactNative.Animated.start(
            BsReactNative.Animated.spring(
              ~value=animatedValue,
              ~velocity=e##velocityX,
              ~useNativeDriver=true,
              ~toValue=`raw(toValue),
              (),
            ),
            ~callback=
              _end_ =>
                if (toValue != 0.
                    && self.ReactUpdate.state.activeScreen
                    - 1 >= 0) {
                  let {screens, activeScreen} = self.ReactUpdate.state;
                  BsReactNative.Animated.Value.setValue(
                    screens[activeScreen - 1].animatedValue,
                    0.0,
                  );
                  BsReactNative.Animated.Value.setValue(
                    screens[activeScreen].animatedValue,
                    1.0,
                  );
                  BsReactNative.Animated.Value.setValue(
                    self.state.headerAnimatedValue,
                    float_of_int(activeScreen - 1),
                  );
                  BsReactNative.Animated.Value.setValue(animatedValue, 0.0);
                  self.ReactUpdate.send(RemoveLastScreen);
                },
            (),
          );
        | _ => ()
        };
      };
    };
    /**
     * Helpers specific to this module
     */
    module Helpers = {
      let isActiveScreen = (state, key) =>
        state.screens[state.activeScreen].key == key;
    };
    /**
     * StackNavigator component
     */
    [@react.component]
    let make =
        (
          ~initialState,
          ~onStateChange=_state => (),
          ~headerComponent=switch (Platform.os) {
                           | _ as os when os == Platform.android => Header.Android.make
                           | _ => Header.IOS.make
                           },
          ~headerMode=switch (Platform.os) {
                      | _ as os when os == Platform.android => Screen
                      | _ => Floating
                      },
          ~onNavigationReady=ignore,
          ~children,
        ) => {
      let (state, send) =
        ReactUpdate.useReducer(
          {
            let activeScreen = Array.length(initialState) - 1;
            {
              pendingTransition: None,
              headerAnimatedValue:
                BsReactNative.Animated.Value.create(
                  float_of_int(activeScreen),
                ),
              screens:
                initialState
                |> Array.mapi((idx, route) =>
                     {
                       route,
                       header: Header.default,
                       animation: Animation.default,
                       key: UUID.generate(),
                       animatedValue:
                         BsReactNative.Animated.Value.create(
                           idx == activeScreen ? 0.0 : (-1.0),
                         ),
                       style: Styles.card,
                       didMount: false,
                     }
                   ),
              activeScreen,
            };
          },
          (action, state) =>
          switch (action) {
          /***
           * Starts Animated transition between two screens
           *
           * The animation is configured based on the latter screen. That said,
           * when screen B (being removed) uses `fade` transition, the screen
           * that is to appear will fade in (even though it doesn't define custom
           * animation itself).
           *
           * Values -1, 0, 1 describe position on screen. Screen with value `0` is the
           * one that is currently visible. Screen with "1" is rendered and hidden on the
           * right hand side whereas "-1" is hidden on the left hand side.
           *
           * Example:
           * 0 to -1 -> next screen has been pushed
           * 0 to 1 -> this screen is getting popped
           * -1 to 0 -> next screen has been popped
           * 1 to 0 -> this screen has been pushed
           */
          | StartTransition(transition, fromIdx, toIdx) =>
            SideEffects(
              self => {
                let (first, second) =
                  transition == `Push || transition == `Replace
                    ? (state.screens[fromIdx], state.screens[toIdx])
                    : (state.screens[toIdx], state.screens[fromIdx]);
                let (fstValues, sndValues) =
                  switch (transition) {
                  | `Push => ((0.0, (-1.0)), (1.0, 0.0))
                  | `Replace => ((0.0, (-1.0)), (1.0, 0.0))
                  | `Pop => (((-1.0), 0.0), (0.0, 1.0))
                  };
                /**
                 * There seems to be a bug with `Animated` that it resets
                 * Animated.Values to its initial values after the transition finishes.
                 *
                 * Since `bs-react-native` doesn't currently support `fromValue` attribute,
                 * we explicitly setValues before starting a new animation.
                 */
                BsReactNative.Animated.Value.setValue(
                  first.animatedValue,
                  fstValues |> fst,
                );
                BsReactNative.Animated.Value.setValue(
                  second.animatedValue,
                  sndValues |> fst,
                );
                BsReactNative.Animated.start(
                  BsReactNative.Animated.parallel(
                    [|
                      second.animation.func(
                        ~value=Gestures.animatedValue,
                        ~toValue=`raw(0.0),
                      ),
                      second.animation.func(
                        ~value=state.headerAnimatedValue,
                        ~toValue=`raw(float_of_int(toIdx)),
                      ),
                      second.animation.func(
                        ~value=first.animatedValue,
                        ~toValue=`raw(fstValues |> snd),
                      ),
                      second.animation.func(
                        ~value=second.animatedValue,
                        ~toValue=`raw(sndValues |> snd),
                      ),
                    |],
                    {"stopTogether": false},
                  ),
                  ~callback=
                    end_ =>
                      switch (transition) {
                      | `Pop when end_##finished =>
                        self.send(RemoveStaleScreen(second.key))
                      | `Replace when end_##finished =>
                        self.send(ReplaceScreenFinish(first.key))
                      | _ => ()
                      },
                  (),
                );
                None;
              },
            )
          | ReplaceScreen(route, key) =>
            if (Helpers.isActiveScreen(state, key)) {
              let index = state.activeScreen + 1;
              let oldActiveScreen = state.activeScreen;
              UpdateWithSideEffects(
                {
                  ...state,
                  activeScreen: index,
                  screens:
                    state.screens
                    |> ReArray.append(
                         {
                           route,
                           header: Header.default,
                           animation: Animation.default,
                           animatedValue:
                             BsReactNative.Animated.Value.create(1.0),
                           key: UUID.generate(),
                           didMount: false,
                           style: Styles.card,
                         },
                         index,
                       ),
                },
                self => {
                  self.send(
                    StartTransition(
                      `Replace,
                      oldActiveScreen,
                      self.state.activeScreen,
                    ),
                  );
                  None;
                },
              );
            } else {
              ReactUpdate.NoUpdate;
            }
          /***
           * Rearranges the index after removing the replaced screen
           */
          | ReplaceScreenFinish(key) =>
            Update({
              ...state,
              activeScreen: state.activeScreen - 1,
              screens:
                state.screens
                ->Belt.Array.keep((screen: screenConfig) =>
                    screen.key !== key
                  ),
            })
          /***
           * Pushes new screen onto the stack
           *
           * Note: We push the item right after the active one (instead of always
           * adding to the end). This is to make sure no glitches happen when you
           * push in the middle of a pop.
           */
          | PushScreen(route, key) =>
            if (Helpers.isActiveScreen(state, key)) {
              let index = state.activeScreen + 1;
              UpdateWithSideEffects(
                {
                  ...state,
                  activeScreen: index,
                  screens:
                    state.screens
                    |> ReArray.append(
                         {
                           route,
                           header: Header.default,
                           animation: Animation.default,
                           animatedValue:
                             BsReactNative.Animated.Value.create(1.0),
                           key: UUID.generate(),
                           didMount: false,
                           style: Styles.card,
                         },
                         index,
                       ),
                },
                self => {
                  self.send(
                    StartTransition(
                      `Push,
                      index - 1,
                      self.state.activeScreen,
                    ),
                  );
                  None;
                },
              );
            } else {
              ReactUpdate.NoUpdate;
            }
          /***
           * Pops screen from the stack
           */
          | PopScreen(key) =>
            let activeScreen = state.activeScreen;
            if (state.activeScreen > 0 && Helpers.isActiveScreen(state, key)) {
              UpdateWithSideEffects(
                {
                  ...state,
                  pendingTransition: None,
                  activeScreen: activeScreen - 1,
                },
                self => {
                  self.send(
                    StartTransition(`Pop, activeScreen, activeScreen - 1),
                  );
                  None;
                },
              );
            } else {
              ReactUpdate.NoUpdate;
            };
          /***
           * Removes a stale screen from the stack w/o animation.
           *
           * This is usually done when the pop animation of particular screen
           * finishes and the screen is no longer within the viewport.
           */
          | RemoveStaleScreen(key) =>
            Update({
              ...state,
              screens:
                state.screens
                ->Belt.Array.keep((screen: screenConfig) =>
                    screen.key !== key
                  ),
            })
          | RemoveLastScreen =>
            Update({
              ...state,
              pendingTransition: None,
              activeScreen: state.activeScreen - 1,
              screens: state.screens |> ReArray.remove(state.activeScreen),
            })
          /***
           * Sets option for a screen with a given key.
           *
           * First time options are called, we assume the screen mounted.
           * This is to notify the animation controller that it's now
           * ready to go.
           */
          | SetOptions({header, animation, style}, key) =>
            let screens = Js.Array.copy(state.screens);
            let idx =
              screens
              |> Js.Array.findIndex((screen: screenConfig) =>
                   screen.key == key
                 );
            screens[idx] = {
              ...screens[idx],
              header,
              style: style |> Js.Option.getWithDefault(screens[idx].style),
              didMount: true,
              animation:
                animation |> Js.Option.getWithDefault(screens[idx].animation),
            };
            Update({...state, screens});
          }
        );

      React.useEffect0(() => {
        onNavigationReady(
          getNavigationInterface(send, state.screens[state.activeScreen].key),
        );
        None;
      });

      React.useEffect1(
        () => {
          onStateChange(state.screens |> Array.map(screen => screen.route));
          None;
        },
        [|state|],
      );

      let size = Array.length(state.screens);
      let screenWidth = Dimensions.get(`window)##width;
      /**
         * Animation for a screen is always defined by the one that is after it.
         */
      let getAnimation = (idx, screens: array(screenConfig)) =>
        idx + 1 == size ? screens[idx].animation : screens[idx + 1].animation;
      /**
         * Aquapoint is the distance between parent and its sibling
         * used by default on iOS (auto-layout constraint). This is
         * the used for defining how far from the screen your gesture
         * can start.
         *
         * Source: https://goo.gl/FVKnzZ
         */
      let aquaPoint = 20;
      let headerProps: Header.props = {
        activeScreen: state.activeScreen,
        animatedValue:
          BsReactNative.Animated.Value.add(
            state.headerAnimatedValue,
            BsReactNative.Animated.Value.multiply(
              Gestures.animatedProgress,
              BsReactNative.Animated.Value.create(-1.0),
            ),
          ),
        pop: key => send(PopScreen(key)),
        screens:
          state.screens
          |> Array.mapi((idx, screen: screenConfig) =>
               {
                 Header.header: screen.header,
                 animation: getAnimation(idx, state.screens),
                 key: screen.key,
               }
             ),
      };

      let makeHeaderProps =
        switch (Platform.os) {
        | _ as os when os == Platform.android => Header.Android.makeProps
        | _ => Header.IOS.makeProps
        };

      <View style=Styles.stackContainer>
        <Gestures.PanHandler
          minDeltaX=aquaPoint
          hitSlop={"right": aquaPoint - int_of_float(screenWidth)}
          maxDeltaX=screenWidth
          enabled={size > 1}
          onGestureEvent=Gestures.handler
          onHandlerStateChange=Gestures.onStateChange>
          <Animated.View style=Styles.flex>
            {state.screens
             |> Array.mapi((idx, screen: screenConfig) => {
                  let animation =
                    if (size == 1) {
                      Style.style([]);
                    } else {
                      BsReactNative.Animated.Value.add(
                        Gestures.animatedProgress,
                        screen.animatedValue,
                      )
                      |> getAnimation(idx, state.screens).forCard;
                    };
                  <Animated.View
                    key={screen.key}
                    style=Style.(list([Styles.fill, screen.style, animation]))>
                    {headerMode == Screen
                       ? headerComponent(
                           makeHeaderProps(
                             ~headerProps={...headerProps, activeScreen: idx},
                             (),
                           ),
                         )
                       : <View />}
                    {children(
                       ~currentRoute=screen.route,
                       ~navigation=getNavigationInterface(send, screen.key),
                     )}
                  </Animated.View>;
                })
             |> React.array}
          </Animated.View>
        </Gestures.PanHandler>
        {headerMode == Floating
           ? headerComponent(makeHeaderProps(~headerProps, ())) : <View />}
      </View>;
    };
    module Screen = {
      let flexOne = Style.(style([flex(1.)]));
      [@react.component]
      let make =
          (
            ~navigation: navigation,
            ~style=?,
            ~headerTitle=?,
            ~headerStyle=?,
            ~headerLeft=?,
            ~headerCenter=?,
            ~headerRight=?,
            ~animation=?,
            ~children,
            (),
          ) => {
        React.useEffect0(() => {
          navigation.setOptions({
            header: {
              title: headerTitle,
              style: headerStyle,
              center: headerCenter,
              left: headerLeft,
              right: headerRight,
            },
            animation,
            style,
          });
          None;
        });
        let body = children();
        <View style=flexOne> body </View>;
      };
    };
  };
};
