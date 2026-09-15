import lodash from "lodash";
window._ = lodash;

import "../css/app.css";
import { createApp } from "vue";
import App from './App.vue';
import components from "./components";

const app = createApp(App)
    .use(components)
    .mount("#app");