// Lodash
window._ = require('lodash');

// Vue
import { createApp } from "vue";
import App from './App.vue';

// Init app
window.app = createApp(App);

require("./helpers.js");

const components = require.context("./components/", true, /\.vue$/i);
components.keys().map(key => app.component(key.split("/").pop().split(".")[0], components(key).default));

window.app.mount("#app");