window.app.config.globalProperties.$get = _.get;
window.app.config.globalProperties.$set = _.set;
window.app.config.globalProperties.$round = _.round;

window.app.config.globalProperties.$copyToClipboard = function(str) {
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

window.app.config.globalProperties.$nullRound = function(value, dec) {
    if (value === null || isNaN(value))
        return "-";
    return _.round(value, dec);
};