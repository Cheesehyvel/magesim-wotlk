import Vue from "vue";

Vue.prototype.$get = _.get;
Vue.prototype.$set = _.set;
Vue.prototype.$round = _.round;

Vue.prototype.$copyToClipboard = function(str) {
    var el = document.createElement("textarea");
    el.value = str;
    el.style.opacity = 0;
    el.style.position = "absolute";
    el.style.top = 0;
    document.body.appendChild(el);
    el.select();
    document.execCommand('copy')
    document.body.removeChild(el);
};

Vue.prototype.$nullRound = function(value, dec) {
    if (value === null || isNaN(value))
        return "-";
    return _.round(value, dec);
};