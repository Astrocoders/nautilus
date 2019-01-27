module Scene = {
  [@bs.module "react-native-magic-move"]
  external view: ReasonReact.reactClass = "Scene";

  let component = ReasonReact.statelessComponent("Scene");
  let make = children =>
    ReasonReact.wrapJsForReason(
      ~reactClass=view,
      ~props=Js.Obj.empty(),
      children,
    );
};

module View = {
  [@bs.module "react-native-magic-move"]
  external view: ReasonReact.reactClass = "View";

  let component = ReasonReact.statelessComponent("View");
  let make = (~id: string, ~style: BsReactNative.Style.t, children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=view,
      ~props={"id": id, "style": style},
      children,
    );
};

module Provider = {
  [@bs.module "react-native-magic-move"]
  external view: ReasonReact.reactClass = "Provider";

  let component = ReasonReact.statelessComponent("Provider");
  let make = (children) =>
    ReasonReact.wrapJsForReason(
      ~reactClass=view,
      ~props=Js.Obj.empty(),
      children,
    );
};
