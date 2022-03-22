// Lodash
window._ = require('lodash');

// Vue
import Vue from "vue";

require("./helpers.js");

const components = require.context("./components/", true, /\.vue$/i);
components.keys().map(key => Vue.component(key.split("/").pop().split(".")[0], components(key).default));

import App from './App.vue';
const AppConstructor = Vue.extend(App);

// Init app
window.app = new AppConstructor({
    el: "#app",
});