/**
 * Animation type
 *
 * Lots of logic used for `forX` interpolators has been inspired and borrowed by
 * great work by React Navigation team.
 *
 * You can browse their implementation on Github:
 * - /src/views/Header/HeaderStyleInterpolator.js
 */
type t = {
  func:
    (
      ~value: BsReactNative.Animated.value(BsReactNative.Animated.regular),
      ~toValue: [
                  | `animated(
                      BsReactNative.Animated.value(
                        BsReactNative.Animated.regular,
                      ),
                    )
                  | `raw(float)
                ]
    ) =>
    BsReactNative.Animated.Animation.t,
  forCard: 'a. BsReactNative.Animated.value('a) => BsReactNative.Style.t,
  forHeader: 'a. BsReactNative.Animated.value('a) => BsReactNative.Style.t,
};

/**
 * Slide in/out animation modelled after iOS platform interactions
 */
let slideHorizontal: t;

let fadeVertical: t;

let fade: t;

/**
 * Platform-specific default animation that is picked by navigators when
 * nothing else is set
 */
let default: t;

/**
 * No animation
 */
let none: t;
