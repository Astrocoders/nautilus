# Nautilus

[![Build Status][build-badge]][build]
[![Version][version-badge]][package]
[![MIT License][license-badge]][license]
[![PRs Welcome][prs-welcome-badge]][prs-welcome]
[![Chat][chat-badge]][chat]

> a fast, declarative navigation for reason react native

### Installation

Open a Terminal in your project's folder and run,

```sh
yarn add @astrocoders/nautilus
```

After installation, you will need to add this library to your `bsconfig.json` dependencies

```json
"bs-dependencies": [
  "reason-react",
  "nautilus"
],
```

In addition you will need to install a few peer dependencies:

```sh
yarn add xxx
```

* `react-native-gesture-handler` - [installation instructions](https://github.com/kmagiera/react-native-gesture-handler#installation)

As per [this issue](https://github.com/callstackincubator/rebolt-navigation/issues/103), you might need to add the following to your `bsconfig.json`:

```

"package-specs": {
  "module": "commonjs",
  "in-source": true
}

```
This generates output alongside source files. ([documentation](https://bucklescript.github.io/docs/en/build-configuration.html#package-specs))


## Usage

create the docs

## Examples


## Developing

```sh
# Starts Metro (React Native Packager)
npm run start-example

# Runs the app on iOS/Android
yarn run-ios

# Watches for source build changes
yarn start
```

## License

MIT (c) 2018 Astrocoders

Badges to Update
<!-- badges -->
[build-badge]: https://img.shields.io/circleci/project/github/callstackincubator/rebolt-navigation/master.svg?style=flat-square
[build]: https://circleci.com/gh/callstackincubator/rebolt-navigation
[version-badge]: https://img.shields.io/npm/v/rebolt-navigation.svg?style=flat-square
[package]: https://www.npmjs.com/package/rebolt-navigation
[license-badge]: https://img.shields.io/npm/l/rebolt-navigation.svg?style=flat-square
[license]: https://opensource.org/licenses/MIT
[prs-welcome-badge]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square
[prs-welcome]: http://makeapullrequest.com
[chat-badge]: https://img.shields.io/discord/426714625279524876.svg?style=flat-square&colorB=758ED3
[chat]: https://discord.gg/zwR2Cdh
