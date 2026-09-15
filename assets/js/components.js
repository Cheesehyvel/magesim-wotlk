import CheckItem from "./components/CheckItem.vue";
import Help from "./components/Help.vue";
import Histogram from "./components/Histogram.vue";
import Loader from "./components/Loader.vue";
import SortLink from "./components/SortLink.vue";
import Timeline from "./components/Timeline.vue";
import TimingHelper from "./components/TimingHelper.vue";
import Tooltip from "./components/Tooltip.vue";

export default {
    install(app) {
        app.component("CheckItem", CheckItem);
        app.component("Help", Help);
        app.component("Histogram", Histogram);
        app.component("Loader", Loader);
        app.component("SortLink", SortLink);
        app.component("Timeline", Timeline);
        app.component("TimingHelper", TimingHelper);
        app.component("Tooltip", Tooltip);
    }
};