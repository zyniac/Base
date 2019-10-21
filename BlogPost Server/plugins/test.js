/// <reference path="../lib.zyniac.d.ts" />

const PLUGIN_NAME = "Zyniac Test Plugin";

console.writeLine("Plugin " + PLUGIN_NAME + " loaded.");

const content = WebServer.registerContent('web/index.html');
const dt = "different Thread";

let i = 0;

content.addEventListener('call', function(event) {
    i++;
    console.log("Point in Javascript in " + dt + " reached");
    event.setContent("test content: " + i);
    event.setMIME("text/plain");
    return true;
});