/// <reference path="lib.zyniac.d.ts" />
// Script is not valid
// This should be added later.

const content = Web.Server.registerContent('plugin/example');
content.addEventListener('call', function(event) {
    event.setContent("My Content has a higher priority than the real content");
    event.setMIME("text/plain");
});

content.addEventListener('construct', function(event) {
    
});