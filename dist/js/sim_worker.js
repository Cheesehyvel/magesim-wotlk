/******/ (() => { // webpackBootstrap
var __webpack_exports__ = {};
/*!*********************************!*\
  !*** ./assets/js/sim_worker.js ***!
  \*********************************/
importScripts("./magesim.js");

onmessage = function onmessage(event) {
  var data = event.data;

  if (data.type == "start") {
    var wasm = fetch("./magesim.wasm", {
      cache: "no-store"
    }).then(function (r) {
      return r.arrayBuffer();
    }).then(function (binary) {
      return MageSim({
        wasmBinary: binary
      });
    }).then(function (w) {
      return w.ready;
    }).then(function (m) {
      var config = m.allocConfig();

      for (var key in data.config) {
        if (key.indexOf("_t") && Array.isArray(data.config[key])) {
          var v = new m["VectorDouble"]();

          for (var i = 0; i < data.config[key].length; i++) {
            if (typeof data.config[key][i] == "number") v.push_back(data.config[key][i]);else break;
          }

          config[key] = v;
        } else if (typeof config[key] != "undefined") {
          config[key] = data.config[key];
        }
      }

      if (m.Rotation.values.hasOwnProperty(data.config.rotation)) config.rotation = m.Rotation.values[data.config.rotation];
      if (m.Trinket.values.hasOwnProperty(data.config.trinket1)) config.trinket1 = m.Trinket.values[data.config.trinket1];
      if (m.Trinket.values.hasOwnProperty(data.config.trinket2)) config.trinket2 = m.Trinket.values[data.config.trinket2];
      if (m.MetaGem.values.hasOwnProperty(data.config.meta_gem)) config.meta_gem = m.MetaGem.values[data.config.meta_gem];
      if (m.Drums.values.hasOwnProperty(data.config.drums)) config.drums = m.Drums.values[data.config.drums];
      if (m.Potion.values.hasOwnProperty(data.config.potion)) config.potion = m.Potion.values[data.config.potion];
      if (m.Potion.values.hasOwnProperty(data.config.pre_potion)) config.pre_potion = m.Potion.values[data.config.pre_potion];
      if (m.Conjured.values.hasOwnProperty(data.config.conjured)) config.conjured = m.Conjured.values[data.config.conjured];
      var player = m.allocPlayer(config);
      if (m.Race.values.hasOwnProperty(data.config.race)) player.race = m.Race.values[data.config.race];
      var stats = JSON.parse(JSON.stringify(player.getStats()));

      for (var key in data.config.stats) {
        if (stats.hasOwnProperty(key)) stats[key] = data.config.stats[key];
      }

      player.setStats(stats);
      var talents = JSON.parse(JSON.stringify(player.getTalents()));

      for (var key in data.config.talents) {
        if (talents.hasOwnProperty(key)) talents[key] = data.config.talents[key];
      }

      player.setTalents(talents);
      var glyphs = JSON.parse(JSON.stringify(player.getGlyphs()));

      for (var key in data.config.glyphs) {
        if (glyphs.hasOwnProperty(key)) glyphs[key] = data.config.glyphs[key];
      }

      player.setGlyphs(glyphs);
      if (data.iterations && data.iterations > 1) var result = m.runSimulations(config, player, data.iterations);else var result = m.runSimulation(config, player);
      if (result.log) result.log = JSON.parse(result.log);
      if (result.spells) result.spells = JSON.parse(result.spells);
      if (result.histogram) result.histogram = JSON.parse(result.histogram);
      if (result.stats) result.stats = JSON.parse(result.stats);
      postMessage({
        type: "success",
        result: result
      });
    })["catch"](function (e) {
      return console.error(e);
    });
  }
};
/******/ })()
;