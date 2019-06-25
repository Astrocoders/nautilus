open ReactNative;
module Style = BsReactNative.Style;
module Animated = BsReactNative.Animated;

type t = {
  forHeaderCenter: 'a. BsReactNative.Animated.value('a) => Style.t,
  forHeaderLeft: 'a. BsReactNative.Animated.value('a) => Style.t,
  forHeaderLeftLabel: 'a. BsReactNative.Animated.value('a) => Style.t,
  forHeaderLeftButton: 'a. BsReactNative.Animated.value('a) => Style.t,
  forHeaderRight: 'a. BsReactNative.Animated.value('a) => Style.t,
};

let crossFadeInterpolation = (~startRange, ~midRange, ~endRange, value) =>
  Animated.Value.interpolate(
    value,
    ~inputRange=[
      startRange,
      startRange +. 0.001,
      midRange -. 0.9,
      midRange -. 0.2,
      midRange,
      endRange -. 0.001,
      endRange,
    ],
    ~outputRange=`float([0.0, 0.0, 0.0, 0.3, 1.0, 0.0, 0.0]),
    (),
  );

let floating = {
  forHeaderCenter: value => {
    let offset = (Dimensions.get(`window)##width /. 2. -. 70. +. 25.);
    BsReactNative.Style.(
      style([
        opacity(
          Animated(
            BsReactNative.Animated.Value.interpolate(
              value,
              ~inputRange=[(-1.0), (-0.999), (-0.5), 0.0, 0.7, 0.999, 1.0],
              ~outputRange=`float([0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0]),
              (),
            ),
          ),
        ),
        Transform.makeAnimated(
          ~translateX=
            Animated.Value.interpolate(
              value,
              ~inputRange=[(-1.0), (-0.99), 0.0, 0.99, 1.0],
              ~outputRange=
                `float([offset, offset, 0.0, -. offset, -. offset]),
              (),
            ),
          (),
        ),
      ])
    );
  },
  forHeaderLeft: _value => BsReactNative.Style.style([]),
  forHeaderLeftButton: value =>
    Style.(
      style([
        opacity(
          Animated(
            Animated.Value.interpolate(
              value,
              ~inputRange=[(-1.0), (-0.999), (-0.5), 0.0, 0.5, 0.999, 1.0],
              ~outputRange=`float([0.0, 0.0, 0.7, 1.0, 0.7, 0.0, 0.0]),
              (),
            ),
          ),
        ),
      ])
    ),
  forHeaderLeftLabel: value => {
    let offset = (Dimensions.get(`window)##width /. 2. -. 70. +. 25.);
    Style.(
      style([
        opacity(
          Animated(
            Animated.Value.interpolate(
              value,
              ~inputRange=[(-1.0), (-0.999), (-0.35), 0.0, 0.5, 0.999, 1.0],
              ~outputRange=`float([0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0]),
              (),
            ),
          ),
        ),
        Transform.makeAnimated(
          ~translateX=
            Animated.Value.interpolate(
              value,
              ~inputRange=[(-1.0), (-0.999), 0.0, 0.999, 1.0],
              ~outputRange=
                `float([
                  offset,
                  offset,
                  0.0,
                  -. offset *. 1.5,
                  -. offset *. 1.5,
                ]),
              (),
            ),
          (),
        ),
      ])
    );
  },
  forHeaderRight: value =>
    Style.(
      style([
        opacity(
          Animated(
            value
            |> crossFadeInterpolation(
                 ~startRange=-1.0,
                 ~midRange=0.0,
                 ~endRange=1.0,
               ),
          ),
        ),
      ])
    ),
};
