module Easing = {
  type t = float => float;
  [@bs.module "react-native"] [@bs.scope "Easing"] external bounce: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external circle: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external cubic: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external ease: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external exp: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external linear: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external poly: float => t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external quad: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external sin: t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external step0: float => int = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external step1: float => int = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external back: float => t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external elastic: float => t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external in_: t => t = "in";
  [@bs.module "react-native"] [@bs.scope "Easing"] external inOut: t => t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"] external out: t => t = "";
  [@bs.module "react-native"] [@bs.scope "Easing"]
  external bezier: (float, float, float, float) => t = "";
};

module AnimatedRe = {
  module Animation = {
    type t;
    type endResult = {. "finished": bool};
    type endCallback = endResult => unit;
    [@bs.send]
    external _start: (t, Js.undefined(endCallback)) => unit = "start";
    let start = (t, ~callback=?, ()) =>
      _start(t, Js.Undefined.fromOption(callback));
    [@bs.send] external stop: t => unit = "";
    [@bs.send] external reset: t => unit = "";
  };

  module type Value = {
    type t;
    type rawJsType;
  };

  type calculated;

  type regular;

  type value('a);

  type valueXY;

  module ValueAnimations = (Val: Value) => {
    module Decay = {
      type config;
      [@bs.obj]
      external makeConfig:
        (
          ~velocity: Val.rawJsType,
          ~deceleration: float=?,
          ~isInteraction: bool=?,
          ~useNativeDriver: bool=?,
          ~onComplete: Animation.endCallback=?,
          ~iterations: int=?
        ) =>
        config =
        "";
      [@bs.module "react-native"] [@bs.scope "Animated"]
      external _decay: (Val.t, config) => Animation.t = "decay";
      let animate =
          (
            ~value,
            ~velocity,
            ~deceleration=?,
            ~isInteraction=?,
            ~useNativeDriver=?,
            ~onComplete=?,
            ~iterations=?,
            (),
          ) =>
        _decay(
          value,
          makeConfig(
            ~velocity,
            ~deceleration?,
            ~isInteraction?,
            ~useNativeDriver?,
            ~onComplete?,
            ~iterations?,
          ),
        );
    };
    module Spring = {
      type toValue;
      type config;
      [@bs.obj]
      external makeConfig:
        (
          ~toValue: toValue,
          ~restDisplacementThreshold: float=?,
          ~overshootClamping: bool=?,
          ~restSpeedThreshold: float=?,
          ~velocity: Val.rawJsType=?,
          ~bounciness: float=?,
          ~speed: float=?,
          ~tension: float=?,
          ~friction: float=?,
          ~stiffness: float=?,
          ~mass: float=?,
          ~damping: float=?,
          ~isInteraction: bool=?,
          ~useNativeDriver: bool=?,
          ~onComplete: Animation.endCallback=?,
          ~iterations: int=?
        ) =>
        config =
        "";
      external toValueRaw: Val.rawJsType => toValue = "%identity";
      external toValueAnimated: Val.t => toValue = "%identity";
      [@bs.module "react-native"] [@bs.scope "Animated"]
      external _spring: (Val.t, config) => Animation.t = "spring";
      let animate =
          (
            ~value,
            ~toValue,
            ~restDisplacementThreshold=?,
            ~overshootClamping=?,
            ~restSpeedThreshold=?,
            ~velocity=?,
            ~bounciness=?,
            ~speed=?,
            ~tension=?,
            ~friction=?,
            ~stiffness=?,
            ~mass=?,
            ~damping=?,
            ~isInteraction=?,
            ~useNativeDriver=?,
            ~onComplete=?,
            ~iterations=?,
            (),
          ) =>
        _spring(
          value,
          makeConfig(
            ~toValue=
              switch (toValue) {
              | `raw(x) => toValueRaw(x)
              | `animated(x) => toValueAnimated(x)
              },
            ~restDisplacementThreshold?,
            ~overshootClamping?,
            ~restSpeedThreshold?,
            ~velocity?,
            ~bounciness?,
            ~speed?,
            ~tension?,
            ~friction?,
            ~stiffness?,
            ~mass?,
            ~damping?,
            ~isInteraction?,
            ~useNativeDriver?,
            ~onComplete?,
            ~iterations?,
          ),
        );
    };
    module Timing = {
      type toValue;
      type config;
      [@bs.obj]
      external makeConfig:
        (
          ~toValue: toValue,
          ~easing: Easing.t=?,
          ~duration: float=?,
          ~delay: float=?,
          ~isInteraction: bool=?,
          ~useNativeDriver: bool=?,
          ~onComplete: Animation.endCallback=?,
          ~iterations: int=?
        ) =>
        config =
        "";
      external toValueRaw: Val.rawJsType => toValue = "%identity";
      external toValueAnimated: Val.t => toValue = "%identity";
      [@bs.module "react-native"] [@bs.scope "Animated"]
      external _timing: (Val.t, config) => Animation.t = "timing";
      let animate =
          (
            ~value,
            ~toValue,
            ~easing=?,
            ~duration=?,
            ~delay=?,
            ~isInteraction=?,
            ~useNativeDriver=?,
            ~onComplete=?,
            ~iterations=?,
            (),
          ) =>
        _timing(
          value,
          makeConfig(
            ~toValue=
              switch (toValue) {
              | `raw(x) => toValueRaw(x)
              | `animated(x) => toValueAnimated(x)
              },
            ~easing?,
            ~duration?,
            ~delay?,
            ~isInteraction?,
            ~useNativeDriver?,
            ~onComplete?,
            ~iterations?,
          ),
        );
    };
  };

  module Interpolation = {
    type t = value(calculated);
    type outputRange;
    external outputRangeCreate: 'a => outputRange = "%identity";
    type extrapolate =
      | Extend
      | Clamp
      | Identity;
    let extrapolateString =
      fun
      | Extend => "extend"
      | Clamp => "clamp"
      | Identity => "identity";

    type config;
    [@bs.obj]
    external makeConfig:
      (
        ~inputRange: array(float),
        ~outputRange: outputRange,
        ~easing: Easing.t=?,
        ~extrapolate: string=?,
        ~extrapolateLeft: string=?,
        ~extrapolateRight: string=?
      ) =>
      config =
      "";
    [@bs.send]
    external _interpolate: (value('a), config) => t = "interpolate";
    let interpolate =
        (
          value,
          ~inputRange,
          ~outputRange,
          ~easing=?,
          ~extrapolate=?,
          ~extrapolateLeft=?,
          ~extrapolateRight=?,
          (),
        ) =>
      _interpolate(
        value,
        makeConfig(
          ~inputRange=Array.of_list(inputRange),
          ~outputRange=
            switch (outputRange) {
            | `string((x: list(string))) =>
              outputRangeCreate(Array.of_list(x))
            | `float((x: list(float))) =>
              outputRangeCreate(Array.of_list(x))
            },
          ~easing?,
          ~extrapolate=?extrapolate->Belt.Option.map(extrapolateString),
          ~extrapolateRight=?
            extrapolateRight->Belt.Option.map(extrapolateString),
          ~extrapolateLeft=?
            extrapolateLeft->Belt.Option.map(extrapolateString),
        ),
      );
  };

  module ValueOperations = {
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external add: (value('a), value('b)) => value(calculated) = "";
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external divide: (value('a), value('b)) => value(calculated) = "";
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external multiply: (value('a), value('b)) => value(calculated) = "";
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external modulo: (value('a), float) => value(calculated) = "";
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external subtract: (value('a), value('b)) => value(calculated) = "";
    [@bs.module "react-native"] [@bs.scope "Animated"]
    external diffClamp: (value('a), float, float) => value(calculated) = "";
    let interpolate = Interpolation.interpolate;
  };

  module Value = {
    type t = value(regular);
    type jsValue = {. "value": float};
    type callback = jsValue => unit;
    [@bs.new] [@bs.scope "Animated"] [@bs.module "react-native"]
    external create: float => t = "Value";
    [@bs.send] external setValue: (t, float) => unit = "setValue";
    [@bs.send] external setOffset: (t, float) => unit = "setOffset";
    [@bs.send] external flattenOffset: t => unit = "flattenOffset";
    [@bs.send] external extractOffset: t => unit = "extractOffset";
    [@bs.send] external addListener: (t, callback) => string = "addListener";
    [@bs.send] external removeListener: (t, string) => unit = "removeListener";
    [@bs.send] external removeAllListeners: t => unit = "removeAllListeners";
    [@bs.send]
    external _resetAnimation: (t, Js.Undefined.t(callback)) => unit =
      "resetAnimation";
    [@bs.send]
    external _stopAnimation: (t, Js.Undefined.t(callback)) => unit =
      "stopAnimation";
    let resetAnimation = (value, ~callback=?, ()) =>
      _resetAnimation(value, Js.Undefined.fromOption(callback));
    let stopAnimation = (value, ~callback=?, ()) =>
      _stopAnimation(value, Js.Undefined.fromOption(callback));
    include ValueAnimations({
      type t = value(regular);
      type rawJsType = float;
    });
    include ValueOperations;
  };

  module ValueXY = {
    type t = valueXY;
    type jsValue = {
      .
      "x": float,
      "y": float,
    };
    type callback = jsValue => unit;
    type translateTransform = {
      .
      "translateX": Value.t,
      "translateY": Value.t,
    };
    type layout = {
      .
      "left": Value.t,
      "top": Value.t,
    };
    [@bs.new] [@bs.scope "Animated"] [@bs.module "react-native"]
    external _create: jsValue => t = "ValueXY";
    let create = (~x, ~y) => _create({"x": x, "y": y});
    [@bs.send] external _setValue: (t, jsValue) => unit = "setValue";
    let setValue = (t, ~x, ~y) => _setValue(t, {"x": x, "y": y});
    [@bs.send] external _setOffset: (t, jsValue) => unit = "setOffset";
    let setOffset = (t, ~x, ~y) => _setOffset(t, {"x": x, "y": y});
    [@bs.send] external flattenOffset: t => unit = "flattenOffset";
    [@bs.send] external extractOffset: t => unit = "extractOffset";
    [@bs.send]
    external resetAnimation: (t, option(callback)) => unit = "resetAnimation";
    [@bs.send]
    external stopAnimation: (t, option(callback)) => unit = "stopAnimation";
    [@bs.send] external addListener: (t, callback) => string = "addListener";
    [@bs.send] external removeListener: (t, string) => unit = "removeListener";
    [@bs.send] external removeAllListeners: t => unit = "removeAllListeners";
    [@bs.send] external getLayout: t => layout = "getLayout";
    [@bs.send]
    external getTranslateTransform: t => translateTransform =
      "getTranslateTransform";
    [@bs.get] external getX: t => Value.t = "x";
    [@bs.get] external getY: t => Value.t = "y";
    include ValueAnimations({
      type t = valueXY;
      type rawJsType = jsValue;
    });
  };

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external delay: float => Animation.t = "";

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external sequence: array(Animation.t) => Animation.t = "";

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external parallel:
    (array(Animation.t), {. "stopTogether": bool}) => Animation.t =
    "";

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external stagger: (float, array(Animation.t)) => Animation.t = "";

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external _loop: (Animation.t, {. "iterations": int}) => Animation.t =
    "loop";

  let loop = (~iterations=(-1), ~animation, ()) =>
    _loop(animation, {"iterations": iterations});

  type animatedEvent;

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external event: (array('a), 'b) => animatedEvent = "";

  [@bs.module "react-native"] [@bs.scope "Animated"]
  external createAnimatedComponent:
    ReasonReact.reactClass => ReasonReact.reactClass =
    "";

  let timing = Value.Timing.animate;

  let spring = Value.Spring.animate;

  let decay = Value.Decay.animate;

  let start = Animation.start;

  let stop = Animation.stop;

  let reset = Animation.reset;
};

/* encoders to json for react-native */
module Internals = {
  /* encoders to json for react-native */
  module Encoder = {
    external string: string => Js.Json.t = "%identity";

    external float: float => Js.Json.t = "%identity";

    external int: int => Js.Json.t = "%identity";

    external boolean: bool => Js.Json.t = "%identity";

    external object_: Js_dict.t(Js.Json.t) => Js.Json.t = "%identity";

    external array: array(Js.Json.t) => Js.Json.t = "%identity";

    external animatedValue: AnimatedRe.value('a) => Js.Json.t = "%identity";

    let pct = f => string(Printf.sprintf("%.2f%%", f));

    let deg = f => string(Printf.sprintf("%.2f%%", f) ++ "deg");
  };
};

type t = ReactNative.Style.t;

type styleElement = (string, Js.Json.t);

type styleTransformElement = (string, Js.Json.t);

type pt_only =
  | Pt(float);

type pt_pct =
  | Pt(float)
  | Pct(float);

let encode_pt_pct =
  fun
  | Pt(value) => Internals.Encoder.float(value)
  | Pct(value) => Internals.Encoder.pct(value);

type pt_pct_auto =
  | Pt(float)
  | Pct(float)
  | Auto;

let encode_pt_pct_auto = value =>
  switch (value) {
  | Pt(pt) => Internals.Encoder.float(pt)
  | Pct(pct) => Internals.Encoder.pct(pct)
  | Auto => Internals.Encoder.string("auto")
  };

type pt_pct_animated('a) =
  | Pt(float)
  | Pct(float)
  | Animated(AnimatedRe.value('a));

let encode_pt_pct_animated =
  fun
  | Pt(value) => Internals.Encoder.float(value)
  | Pct(value) => Internals.Encoder.pct(value)
  | Animated(value) => Internals.Encoder.animatedValue(value);

type float_animated('a) =
  | Float(float)
  | Animated(AnimatedRe.value('a));

let encode_float_animated =
  fun
  | Float(value) => Internals.Encoder.float(value)
  | Animated(value) => Internals.Encoder.animatedValue(value);

type string_interpolated =
  | String(string)
  | Animated(AnimatedRe.Interpolation.t);

let encode_string_interpolated =
  fun
  | String(value) => Internals.Encoder.string(value)
  | Animated(value) => Internals.Encoder.animatedValue(value);

type deg_animated('a) =
  | Deg(float)
  | Animated(AnimatedRe.value('a));

let encode_deg_animated =
  fun
  | Deg(value) => Internals.Encoder.deg(value)
  | Animated(value) => Internals.Encoder.animatedValue(value);

external to_style: Js.Dict.t(Js.Json.t) => t = "%identity";

external style_to_dict: t => Js.Dict.t(Js.Json.t) = "%identity";

external arrayOfStyle: array(t) => t = "%identity";

let floatStyle = (key, value) => (key, Internals.Encoder.float(value));

let stringStyle = (key, value) => (key, Internals.Encoder.string(value));

let objectStyle = (key, value) => (key, Internals.Encoder.object_(value));

let arrayStyle = (key, value) => (key, Internals.Encoder.array(value));

let style = sarr => sarr |> Js.Dict.fromList |> to_style;

external array: array(t) => t = "%identity";
external arrayOption: array(option(t)) => t = "%identity";
external list: list(t) => t = "%identity";
external listOption: list(option(t)) => t = "%identity";

/* deprecated */
let flatten = array;
let concat = styles => styles->Belt.List.toArray->arrayOfStyle;
let combine = (a, b) => {
  let entries =
    Array.append(
      Js.Dict.entries(style_to_dict(a)),
      Js.Dict.entries(style_to_dict(b)),
    );
  Js.Dict.fromArray(entries) |> to_style;
};

/***
 * Layout Props
 */
type alignContent =
  | FlexStart
  | FlexEnd
  | Center
  | Stretch
  | SpaceAround
  | SpaceBetween;

let alignContent = v =>
  stringStyle(
    "alignContent",
    switch (v) {
    | FlexStart => "flex-start"
    | FlexEnd => "flex-end"
    | Center => "center"
    | Stretch => "stretch"
    | SpaceAround => "space-around"
    | SpaceBetween => "space-between"
    },
  );

type alignItems =
  | FlexStart
  | FlexEnd
  | Center
  | Stretch
  | Baseline;

let alignItems = v =>
  stringStyle(
    "alignItems",
    switch (v) {
    | FlexStart => "flex-start"
    | FlexEnd => "flex-end"
    | Center => "center"
    | Stretch => "stretch"
    | Baseline => "baseline"
    },
  );

type alignSelf =
  | FlexStart
  | FlexEnd
  | Center
  | Stretch
  | Baseline;

let alignSelf = v =>
  stringStyle(
    "alignSelf",
    switch (v) {
    | FlexStart => "flex-start"
    | FlexEnd => "flex-end"
    | Center => "center"
    | Stretch => "stretch"
    | Baseline => "baseline"
    },
  );

let aspectRatio = floatStyle("aspectRatio");

let borderBottomWidth = floatStyle("borderBottomWidth");

let borderLeftWidth = floatStyle("borderLeftWidth");

let borderRightWidth = floatStyle("borderRightWidth");

let borderTopWidth = floatStyle("borderTopWidth");

let borderWidth = floatStyle("borderWidth");

type display =
  | Flex
  | None;

let display = v =>
  stringStyle(
    "display",
    switch (v) {
    | Flex => "flex"
    | None => "none"
    },
  );

let flex = floatStyle("flex");

let flexBasis = value => ("flexBasis", encode_pt_pct_auto(value));

type flexDirection =
  | Row
  | RowReverse
  | Column
  | ColumnReverse;

let flexDirection = v =>
  stringStyle(
    "flexDirection",
    switch (v) {
    | Row => "row"
    | RowReverse => "row-reverse"
    | Column => "column"
    | ColumnReverse => "column-reverse"
    },
  );

let flexGrow = floatStyle("flexGrow");

let flexShrink = floatStyle("flexShrink");

type flexWrap =
  | Wrap
  | Nowrap;

let flexWrap = v =>
  stringStyle(
    "flexWrap",
    switch (v) {
    | Wrap => "wrap"
    | Nowrap => "nowrap"
    },
  );

type justifyContent =
  | FlexStart
  | FlexEnd
  | Center
  | SpaceAround
  | SpaceBetween
  | SpaceEvenly;

let justifyContent = v =>
  stringStyle(
    "justifyContent",
    switch (v) {
    | FlexStart => "flex-start"
    | FlexEnd => "flex-end"
    | Center => "center"
    | SpaceAround => "space-around"
    | SpaceBetween => "space-between"
    | SpaceEvenly => "space-evenly"
    },
  );

let margin = value => ("margin", encode_pt_pct_auto(value));

let marginBottom = value => ("marginBottom", encode_pt_pct_auto(value));

let marginHorizontal = value => (
  "marginHorizontal",
  encode_pt_pct_auto(value),
);

let marginLeft = value => ("marginLeft", encode_pt_pct_auto(value));

let marginRight = value => ("marginRight", encode_pt_pct_auto(value));

let marginTop = value => ("marginTop", encode_pt_pct_auto(value));

let marginVertical = value => ("marginVertical", encode_pt_pct_auto(value));

let maxHeight = value => ("maxHeight", encode_pt_pct(value));

let maxWidth = value => ("maxWidth", encode_pt_pct(value));

let minHeight = value => ("minHeight", encode_pt_pct(value));

let minWidth = value => ("minWidth", encode_pt_pct(value));

type overflow =
  | Visible
  | Hidden
  | Scroll;

let overflow = v =>
  stringStyle(
    "overflow",
    switch (v) {
    | Visible => "visible"
    | Hidden => "hidden"
    | Scroll => "scroll"
    },
  );

let padding = value => ("padding", encode_pt_pct(value));

let paddingBottom = value => ("paddingBottom", encode_pt_pct(value));

let paddingHorizontal = value => ("paddingHorizontal", encode_pt_pct(value));

let paddingLeft = value => ("paddingLeft", encode_pt_pct(value));

let paddingRight = value => ("paddingRight", encode_pt_pct(value));

let paddingTop = value => ("paddingTop", encode_pt_pct(value));

let paddingVertical = value => ("paddingVertical", encode_pt_pct(value));

type position =
  | Absolute
  | Relative;

let position = v =>
  stringStyle(
    "position",
    switch (v) {
    | Absolute => "absolute"
    | Relative => "relative"
    },
  );

let top = value => ("top", encode_pt_pct_animated(value));

let left = value => ("left", encode_pt_pct_animated(value));

let right = value => ("right", encode_pt_pct_animated(value));

let bottom = value => ("bottom", encode_pt_pct_animated(value));

let height = value => ("height", encode_pt_pct_animated(value));

let width = value => ("width", encode_pt_pct_animated(value));

let zIndex = value => ("zIndex", Internals.Encoder.int(value));

type direction =
  | Inherit
  | Ltr
  | Rtl;

let direction = v =>
  stringStyle(
    "direction",
    switch (v) {
    | Inherit => "inherit"
    | Ltr => "ltr"
    | Rtl => "rtl"
    },
  );

/***
 * Shadow Props
 */
let shadowColor = value => (
  "shadowColor",
  encode_string_interpolated(value),
);

let shadowOffset = (~height, ~width) =>
  Js.Dict.fromArray([|
    ("height", Internals.Encoder.float(height)),
    ("width", Internals.Encoder.float(width)),
  |])
  |> objectStyle("shadowOffset");

let shadowOpacity = floatStyle("shadowOpacity");

let shadowRadius = floatStyle("shadowRadius");

let transform = listyle =>
  listyle
  ->Belt.List.map(ts => [ts]->Js.Dict.fromList->Internals.Encoder.object_)
  ->Belt.List.toArray
  |> arrayStyle("transform");

/* transform method */
let perspective = value => ("perspective", encode_float_animated(value));
let translateX = value => ("translateX", encode_float_animated(value));
let translateY = value => ("translateY", encode_float_animated(value));
let scaleX = value => ("scaleX", encode_float_animated(value));
let scaleY = value => ("scaleY", encode_float_animated(value));
let scale = value => ("scale", encode_float_animated(value));
let rotate = value => ("rotate", encode_deg_animated(value));
let rotateX = value => ("rotateX", encode_deg_animated(value));
let rotateY = value => ("rotateY", encode_deg_animated(value));
let rotateZ = value => ("rotateZ", encode_deg_animated(value));
let skewX = value => ("skewX", encode_deg_animated(value));
let skewY = value => ("skewY", encode_deg_animated(value));

module Transform = {
  /* deprecated */
  let perspective = perspective;
  let translateX = translateX;
  let translateY = translateY;
  let scaleX = scaleX;
  let scaleY = scaleY;
  let scale = scale;
  let rotate = rotate;
  let rotateX = rotateX;
  let rotateY = rotateY;
  let rotateZ = rotateZ;
  let skewX = skewX;
  let skewY = skewY;

  let create_ =
      (
        encoder,
        rotationEncoder,
        perspective,
        rotate,
        rotateX,
        rotateY,
        rotateZ,
        scaleX,
        scaleY,
        translateX,
        translateY,
        skewX,
        skewY,
      ) => {
    let opt_values = [|
      ("perspective", encoder(. perspective)),
      ("rotate", rotationEncoder(. rotate)),
      ("rotateX", rotationEncoder(. rotateX)),
      ("rotateY", rotationEncoder(. rotateY)),
      ("rotateZ", rotationEncoder(. rotateZ)),
      ("scaleX", encoder(. scaleX)),
      ("scaleY", encoder(. scaleY)),
      ("translateX", encoder(. translateX)),
      ("translateY", encoder(. translateY)),
      ("skewX", rotationEncoder(. skewX)),
      ("skewY", rotationEncoder(. skewY)),
    |];
    let values =
      Array.fold_right(
        (x, acc) =>
          switch (x) {
          | (key, Some(value)) =>
            let val_ =
              Js.Dict.fromArray([|(key, value)|])
              |> Internals.Encoder.object_;
            [val_, ...acc];
          | _ => acc
          },
        opt_values,
        [],
      );
    Array.of_list(values) |> arrayStyle("transform");
  };
  let make =
      (
        ~perspective=?,
        ~rotate=?,
        ~rotateX=?,
        ~rotateY=?,
        ~rotateZ=?,
        ~scaleX=?,
        ~scaleY=?,
        ~translateX=?,
        ~translateY=?,
        ~skewX=?,
        ~skewY=?,
        (),
      ) =>
    create_(
      (. value) => value->Belt.Option.map(Internals.Encoder.float),
      (. value) => value->Belt.Option.map(Internals.Encoder.string),
      perspective,
      rotate,
      rotateX,
      rotateY,
      rotateZ,
      scaleX,
      scaleY,
      translateX,
      translateY,
      skewX,
      skewY,
    );
  let makeAnimated =
      (
        ~perspective=?,
        ~rotate=?,
        ~rotateX=?,
        ~rotateY=?,
        ~rotateZ=?,
        ~scaleX=?,
        ~scaleY=?,
        ~translateX=?,
        ~translateY=?,
        ~skewX=?,
        ~skewY=?,
        (),
      ) =>
    create_(
      (. value) => value->Belt.Option.map(Internals.Encoder.animatedValue),
      (. value) => value->Belt.Option.map(Internals.Encoder.animatedValue),
      perspective,
      rotate,
      rotateX,
      rotateY,
      rotateZ,
      scaleX,
      scaleY,
      translateX,
      translateY,
      skewX,
      skewY,
    );
};

/***
 * View Props
 */
type backfaceVisibility =
  | Visible
  | Hidden;

let backfaceVisibility = v =>
  stringStyle(
    "backfaceVisibility",
    switch (v) {
    | Visible => "visible"
    | Hidden => "hidden"
    },
  );

let backgroundColor = value => (
  "backgroundColor",
  encode_string_interpolated(value),
);

let borderColor = value => (
  "borderColor",
  encode_string_interpolated(value),
);

let borderTopColor = value => (
  "borderTopColor",
  encode_string_interpolated(value),
);

let borderRightColor = value => (
  "borderRightColor",
  encode_string_interpolated(value),
);

let borderBottomColor = value => (
  "borderBottomColor",
  encode_string_interpolated(value),
);

let borderLeftColor = value => (
  "borderLeftColor",
  encode_string_interpolated(value),
);

let borderRadius = floatStyle("borderRadius");

let borderTopLeftRadius = floatStyle("borderTopLeftRadius");

let borderTopRightRadius = floatStyle("borderTopRightRadius");

let borderBottomLeftRadius = floatStyle("borderBottomLeftRadius");

let borderBottomRightRadius = floatStyle("borderBottomRightRadius");

type borderStyle =
  | Solid
  | Dotted
  | Dashed;

let borderStyle = v =>
  stringStyle(
    "borderStyle",
    switch (v) {
    | Solid => "solid"
    | Dotted => "dotted"
    | Dashed => "dashed"
    },
  );

let opacity = value => ("opacity", encode_float_animated(value));

let elevation = floatStyle("elevation");

/***
 * Text Props
 */
let color = value => ("color", encode_string_interpolated(value));

let fontFamily = stringStyle("fontFamily");

let fontSize = value => ("fontSize", encode_float_animated(value));

type fontStyle =
  | Normal
  | Italic;

let fontStyle = v =>
  stringStyle(
    "fontStyle",
    switch (v) {
    | Normal => "normal"
    | Italic => "italic"
    },
  );

let fontWeight = v =>
  stringStyle(
    "fontWeight",
    switch (v) {
    | `Normal => "normal"
    | `Bold => "bold"
    | `_100 => "100"
    | `_200 => "200"
    | `_300 => "300"
    | `_400 => "400"
    | `_500 => "500"
    | `_600 => "600"
    | `_700 => "700"
    | `_800 => "800"
    | `_900 => "900"
    },
  );

let lineHeight = floatStyle("lineHeight");

type textAlign =
  | Auto
  | Left
  | Right
  | Center
  | Justify;

let textAlign = v =>
  stringStyle(
    "textAlign",
    switch (v) {
    | Auto => "auto"
    | Left => "left"
    | Right => "right"
    | Center => "center"
    | Justify => "justify"
    },
  );

type textDecorationLine =
  | None
  | Underline
  | LineThrough
  | UnderlineLineThrough;

let textDecorationLine = v =>
  stringStyle(
    "textDecorationLine",
    switch (v) {
    | None => "none"
    | Underline => "underline"
    | LineThrough => "line-through"
    | UnderlineLineThrough => "underline line-through"
    },
  );

let textShadowColor = value => (
  "textShadowColor",
  encode_string_interpolated(value),
);

let textShadowOffset = (~height, ~width) =>
  Js.Dict.fromArray([|
    ("height", Internals.Encoder.float(height)),
    ("width", Internals.Encoder.float(width)),
  |])
  |> objectStyle("textShadowOffset");

let textShadowRadius = floatStyle("textShadowRadius");

let includeFontPadding = value => (
  "includeFontPadding",
  Internals.Encoder.boolean(value),
);

type textAlignVertical =
  | Auto
  | Top
  | Bottom
  | Center;

let textAlignVertical = v =>
  stringStyle(
    "textAlignVertical",
    switch (v) {
    | Auto => "auto"
    | Top => "top"
    | Bottom => "bottom"
    | Center => "center"
    },
  );

let fontVariant = fontVariants =>
  fontVariants
  |> Array.of_list
  |> Array.map(Internals.Encoder.string)
  |> arrayStyle("fontVariant");

let letterSpacing = floatStyle("letterSpacing");

let textDecorationColor = value => (
  "textDecorationColor",
  encode_string_interpolated(value),
);

type textDecorationStyle =
  | Solid
  | Double
  | Dotted
  | Dashed;

let textDecorationStyle = v =>
  stringStyle(
    "textDecorationStyle",
    switch (v) {
    | Solid => "solid"
    | Double => "double"
    | Dotted => "dotted"
    | Dashed => "dashed"
    },
  );

type writingDirection =
  | Auto
  | Ltr
  | Rtl;

let writingDirection = v =>
  stringStyle(
    "writingDirection",
    switch (v) {
    | Auto => "auto"
    | Ltr => "ltr"
    | Rtl => "rtl"
    },
  );

type resizeMode =
  | Cover
  | Contain
  | Stretch
  | Repeat
  | Center;

/*** Image props */
let resizeMode = v =>
  stringStyle(
    "resizeMode",
    switch (v) {
    | Cover => "cover"
    | Contain => "contain"
    | Stretch => "stretch"
    | Repeat => "repeat"
    | Center => "center"
    },
  );

let tintColor = value => ("tintColor", encode_string_interpolated(value));

let overlayColor = value => (
  "overlayColor",
  encode_string_interpolated(value),
);

type color =
  | String(string);


module Animated = AnimatedRe
