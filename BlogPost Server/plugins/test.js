/// <reference path="../lib.zyniac.d.ts" />

const PLUGIN_NAME = "Zyniac Test Plugin";

console.writeLine("Plugin " + PLUGIN_NAME + " loaded.");

const content = WebServer.registerContent('web/index.html');
const dt = "different Thread";
content.addEventListener('call', function() {
    console.log("Point in Javascript in " + dt + " reached");
    return false;
});