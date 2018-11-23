open BsReactNative;

open Utils;

let rec find = (arr, statement) =>
  switch (arr) {
  | [||] => None
  | arr =>
    let first = arr->Belt.Array.getUnsafe(0);
    let rest = arr->Belt.Array.sliceToEnd(1);
    statement(first) ? Some(first) : find(rest, statement);
  };
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
  let stackContainer =
    concat([flex, style([flexDirection(ColumnReverse)])]);
};

type sharedElementType =
  | Source
  | Destination;
type commonNavigation('route, 'options, 'screen) = {
  push: 'route => unit,
  replace: 'route => unit,
  setOptions: 'options => unit,
  pop: unit => unit,
  addSharedElement:
    (
      ~name: string,
      ~layout: RNEvent.NativeLayoutEvent.layout,
      ~type_: sharedElementType,
      ~renderChildren: unit => ReasonReact.reactElement
    ) =>
    unit,
  screen: 'screen,
  screens: array('screen),
};

module CreateStackNavigator = (Config: {type route;}) => {
  module StackNavigator = {
    module Animation = Animation;
    type sharedElement = {
      name: string,
      type_: sharedElementType,
      originalCoordinates: RNEvent.NativeLayoutEvent.layout,
      renderChildren: unit => ReasonReact.reactElement,
      getAnimatedStyle:
        (Animated.Value.t, RNEvent.NativeLayoutEvent.layout) => Style.t,
    };
    type screenConfig = {
      route: Config.route,
      key: string,
      animatedValue: Animated.Value.t,
      animation: Animation.t,
      didMount: bool,
      style: Style.t,
      sharedElementAnimatedValue: Animated.Value.t,
      sharedElements: array(sharedElement),
    };
    type headerMode =
      | Screen
      | Floating;
    type pendingTransition = (int, int);
    type state = {
      screens: array(screenConfig),
      pendingTransition: option(pendingTransition),
      activeScreen: int,
    };
    type persistedState = array(Config.route);
    type options = {
      animation: option(Animation.t),
      style: option(Style.t),
    };
    type action =
      | AddSharedElement(string, sharedElement)
      | PushScreen(Config.route, string)
      | ReplaceScreen(Config.route, string)
      | StartTransition([ | `Pop | `Replace | `Push], int, int)
      | SetOptions(options, string)
      | RemoveStaleScreen(string)
      | ReplaceScreenFinish(string)
      | RemoveLastScreen
      | PopScreen(string);
    type navigation = commonNavigation(Config.route, options, screenConfig);
    include Persistence.CreatePersistence({
      type state = persistedState;
    });
    let getNavigationInterface = (send, screenKey, screen, screens) => {
      screen,
      screens,
      push: route => send(PushScreen(route, screenKey)),
      replace: route => send(ReplaceScreen(route, screenKey)),
      pop: () => send(PopScreen(screenKey)),
      setOptions: opts => send(SetOptions(opts, screenKey)),
      addSharedElement: (~name, ~layout, ~type_, ~renderChildren) =>
        send(
          AddSharedElement(
            screenKey,
            {
              name,
              originalCoordinates: layout,
              type_,
              renderChildren,
              getAnimatedStyle: (value, targetLayout) =>
                Style.(
                  style([
                    position(Absolute),
                    opacity(
                      Animated(
                        Animated.Value.interpolate(
                          value,
                          ~inputRange=[0.0, 0.99, 1.],
                          ~outputRange=`float([1., 1., 0.]),
                          (),
                        ),
                      ),
                    ),
                    Transform.makeAnimated(
                      ~translateX=
                        Animated.Value.interpolate(
                          value,
                          ~inputRange=[0.0, 1.],
                          ~outputRange=
                            `float([0., layout.x -. targetLayout.x]),
                          (),
                        ),
                      ~translateY=
                        Animated.Value.interpolate(
                          value,
                          ~inputRange=[0.0, 1.0],
                          ~outputRange=
                            `float([0., layout.y -. targetLayout.y]),
                          (),
                        ),
                      (),
                    ),
                  ])
                ),
            },
          ),
        ),
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
    let component = ReasonReact.reducerComponent("StackNavigator");
    let make =
        (
          ~initialState,
          ~onStateChange=_state => (),
          ~onNavigationReady=ignore,
          ~headerComponent=?,
          children,
        ) => {
      ...component,
      initialState: () => {
        let activeScreen = Array.length(initialState) - 1;
        {
          pendingTransition: None,
          screens:
            initialState
            |> Array.mapi((idx, route) =>
                 {
                   route,
                   animation: Animation.fadeVertical,
                   key: UUID.generate(),
                   animatedValue:
                     Animated.Value.create(
                       idx == activeScreen ? 0.0 : (-1.0),
                     ),
                   style: Styles.card,
                   didMount: false,
                   sharedElementAnimatedValue: Animated.Value.create(0.0),
                   sharedElements: [||],
                 }
               ),
          activeScreen,
        };
      },
      didMount: self =>
        onNavigationReady(
          self.handle((cb, self) =>
            cb(
              getNavigationInterface(
                self.send,
                self.state.screens[self.state.activeScreen].key,
                self.state.screens,
              ),
            )
          ),
        ),
      /***
       * Begin animating two states as soon as the index changes.
       *
       * If screen we are transitioning to didn't mount yet, we delay the transition
       * until that happens.
       */
      didUpdate: ({newSelf: self}) =>
        onStateChange(
          self.state.screens |> Array.map(screen => screen.route),
        ),
      /***
       * StackNavigator router
       *
       * Most actions are indempotent and have `isActiveScreen(state, key)` check
       * to make sure we only accept one action from the screen that changes the
       * state.
       */
      reducer: (action, state) =>
        switch (action) {
        | AddSharedElement(screenKey, sharedElement) =>
          Update({
            ...state,
            screens:
              state.screens
              ->Belt.Array.map(screen =>
                  screen.key == screenKey ?
                    {
                      ...screen,
                      sharedElements:
                        screen.sharedElements
                        ->Belt.Array.concat([|sharedElement|]),
                    } :
                    screen
                ),
          })
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
            (
              self => {
                let (first, second) =
                  transition == `Push || transition == `Replace ?
                    (
                      self.ReasonReact.state.screens[fromIdx],
                      self.state.screens[toIdx],
                    ) :
                    (self.state.screens[toIdx], self.state.screens[fromIdx]);
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
                Animated.Value.setValue(
                  first.animatedValue,
                  fstValues |> fst,
                );
                Animated.Value.setValue(second.animatedValue, 1.0);
                Animated.Value.setValue(
                  first.sharedElementAnimatedValue,
                  0.0,
                );
                Animated.start(
                  Animated.sequence([|
                    Animated.timing(
                      ~duration=600.0,
                      ~useNativeDriver=true,
                      ~value=first.sharedElementAnimatedValue,
                      ~toValue=`raw(1.0),
                      (),
                    ),
                    Animated.parallel(
                      [|
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
                  |]),
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
                ();
              }
            ),
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
                         animation: Animation.default,
                         animatedValue: Animated.Value.create(1.0),
                         key: UUID.generate(),
                         didMount: false,
                         style: Styles.card,
                         sharedElementAnimatedValue:
                           Animated.Value.create(0.0),
                         sharedElements: [||],
                       },
                       index,
                     ),
              },
              (
                self =>
                  self.send(
                    StartTransition(
                      `Replace,
                      oldActiveScreen,
                      self.state.activeScreen,
                    ),
                  )
              ),
            );
          } else {
            ReasonReact.NoUpdate;
          }
        /***
         * Rearranges the index after removing the replaced screen
         */
        | ReplaceScreenFinish(key) =>
          ReasonReact.Update({
            ...state,
            activeScreen: state.activeScreen - 1,
            screens:
              state.screens
              ->Belt.Array.keep((screen: screenConfig) => screen.key !== key),
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
                         animation: Animation.fadeVertical,
                         animatedValue: Animated.Value.create(1.0),
                         key: UUID.generate(),
                         didMount: false,
                         style: Styles.card,
                         sharedElementAnimatedValue:
                           Animated.Value.create(0.0),
                         sharedElements: [||],
                       },
                       index,
                     ),
              },
              (
                self =>
                  self.send(
                    StartTransition(
                      `Push,
                      index - 1,
                      self.state.activeScreen,
                    ),
                  )
              ),
            );
          } else {
            ReasonReact.NoUpdate;
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
              (
                self =>
                  self.send(
                    StartTransition(`Pop, activeScreen, activeScreen - 1),
                  )
              ),
            );
          } else {
            ReasonReact.NoUpdate;
          };
        /***
         * Removes a stale screen from the stack w/o animation.
         *
         * This is usually done when the pop animation of particular screen
         * finishes and the screen is no longer within the viewport.
         */
        | RemoveStaleScreen(key) =>
          ReasonReact.Update({
            ...state,
            screens:
              state.screens
              ->Belt.Array.keep((screen: screenConfig) => screen.key !== key),
          })
        | RemoveLastScreen =>
          ReasonReact.Update({
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
        | SetOptions({animation, style}, key) =>
          let screens = Js.Array.copy(state.screens);
          let idx =
            screens
            |> Js.Array.findIndex((screen: screenConfig) => screen.key == key);
          screens[idx] = {
            ...screens[idx],
            style: style |> Js.Option.getWithDefault(screens[idx].style),
            didMount: true,
            animation:
              animation |> Js.Option.getWithDefault(screens[idx].animation),
          };
          ReasonReact.Update({...state, screens});
        },
      render: self => {
        let size = Array.length(self.state.screens);
        /**
         * Animation for a screen is always defined by the one that is after it.
         */
        let getAnimation = (idx, screens: array(screenConfig)) =>
          idx + 1 == size ?
            screens[idx].animation : screens[idx + 1].animation;
        <View style=Styles.stackContainer>
          <Animated.View style=Styles.flex>
            {
              self.state.screens
              |> Array.mapi((idx, screen: screenConfig) => {
                   let animation =
                     if (size == 1) {
                       Style.style([]);
                     } else {
                       screen.animatedValue
                       |> getAnimation(idx, self.state.screens).forCard;
                     };
                   <Animated.View
                     key={screen.key}
                     style=Style.(
                       concat([Styles.fill, screen.style, animation])
                     )>
                     {
                       screen.sharedElements
                       ->Belt.Array.map(element => {
                           let sharedElements =
                             self.state.screens
                             ->Belt.Array.map(screen => screen.sharedElements)
                             ->Belt.Array.reduce([||], Belt.Array.concat);
                           let targetElement =
                             sharedElements
                             ->find(target =>
                                 target.name == element.name
                                 && target.type_ != element.type_
                               );
                           let style = targetElement->Belt.Option.map(target => element.getAnimatedStyle(screen.sharedElementAnimatedValue, target.originalCoordinates))->Belt.Option.getWithDefault(Style.style([]));
          <Animated.View style>
          {
            element.renderChildren()
          }
          </Animated.View>;
                         })
                       |> ReasonReact.array
                     }
                     {
                       children(
                         ~currentRoute=screen.route,
                         ~navigation=
                           getNavigationInterface(
                             self.send,
                             screen.key,
                             screen,
                             self.state.screens,
                           ),
                       )
                     }
                   </Animated.View>;
                 })
              |> ReasonReact.array
            }
          </Animated.View>
        </View>;
      },
    };
    module Screen = {
      let flexOne = Style.(style([flex(1.)]));
      let component = ReasonReact.statelessComponent("CallstackScreen");
      let make =
          (
            ~navigation: navigation,
            ~style=?,
            ~animation=?,
            /* Remove this */
            ~headerTitle=?,
            children,
          ) => {
        ...component,
        didMount: _self => {
          navigation.setOptions({animation, style});
          ();
        },
        render: _self => {
          let body = children();
          <View style=flexOne> body </View>;
        },
      };
    };
    module SharedElement = {
      type state = {
        nodeRef: option(ReasonReact.reactRef),
        measured: bool,
      };
      type action =
        | Set(option(ReasonReact.reactRef))
        | MeasureLayout;
      let component = ReasonReact.reducerComponent("SharedElement");

      let make = (~navigation: navigation, ~name, ~type_, children) => {
        ...component,
        initialState: () => {nodeRef: None, measured: false},
        reducer: (action, state) =>
          switch (action) {
          | Set(nodeRef) =>
            switch (state.nodeRef) {
            | Some(_) => NoUpdate
            | None => Update({...state, nodeRef})
            }
          | MeasureLayout =>
            UpdateWithSideEffects(
              {...state, measured: true},
              (
                _ =>
                  switch (state.nodeRef) {
                  | Some(nodeRef) =>
                    let onLayout = e =>
                      navigation.addSharedElement(
                        ~name,
                        ~layout=e->RNEvent.NativeLayoutEvent.layout,
                        ~type_,
                        ~renderChildren=children,
                      );
                    let measureLayout:
                      (
                        . ReasonReact.reactRef,
                        RNEvent.NativeLayoutEvent.t => unit
                      ) =>
                      unit = [%bs.raw
                      {|
                      function(ref, cb) {
                        const { UIManager, findNodeHandle } = require('react-native');

                        const node = findNodeHandle(ref);
                        UIManager.measureInWindow(node, (x, y, width, height) => {
                          cb({
                            nativeEvent: {
                              layout: {
                                x,
                                y,
                                width,
                                height,
                              },
                            },
                          })
                        })
                      }
                      |}
                    ];

                    measureLayout(. nodeRef, onLayout);
                  | None =>
                    Js.log(
                      "Attempt to load shared element ref but it was not initialized yet",
                    )
                  }
              ),
            )
          },
        render: self => {
          let targetType = type_ == Source ? Destination : Source;
          let style =
            Belt.Option.getWithDefault(
              {
                let sharedElements =
                  navigation.screens
                  ->Belt.Array.map(screen => screen.sharedElements)
                  ->Belt.Array.reduce([||], Belt.Array.concat);
                let sourceElement =
                  sharedElements
                  ->find(element =>
                      element.name == name && element.type_ == type_
                    );
                let targetElement =
                  sharedElements
                  ->find(element =>
                      element.name == name && element.type_ == targetType
                    );

                switch (sourceElement, targetElement) {
                | (Some(source), Some(target)) =>
                  Some(
                    source.getAnimatedStyle(
                      navigation.screen.sharedElementAnimatedValue,
                      target.originalCoordinates,
                    ),
                  )
                | _ => None
                };
              },
              Style.style([]),
            );

            <View
              ref={
                node =>
                  self.state.nodeRef == None ?
                    self.send(Set(Js.Nullable.toOption(node))) : ()
              }
              onLayout={_ => self.send(MeasureLayout)}>
              {children()}
            </View>
        },
      };
    };
  };
};
