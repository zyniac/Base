let script = null;
let unload = null;

const MNGScriptPublic = {
    headElement: null,
    loadingScreen: null,
    wsTitle: null
}

window.addEventListener('load', function() {
    MNGScriptPublic.headElement = document.getElementsByTagName('head')[0];
    MNGScriptPublic.loadingScreen = document.getElementById('loading-screen');
    MNGScriptPublic.wsTitle = document.getElementById('wsTitle');

    MNGScriptPublic.wsTitle.addEventListener('click', function() {
        window.location = document.URL.split('?')[0];
    });

    let blogId = document.documentURI.split('?')[1];
    if(blogId) {
        blogId = blogId.split('#')[0];
        setLoadingScreen(true, function() {
            loadBlog(decodeURI(blogId));
        });
    } else {
        setLoadingScreen(true, loadHome);
    }
    let bannerNode = document.querySelector('[alt="www.000webhost.com"]').parentNode.parentNode;
    if(bannerNode)
        bannerNode.parentNode.removeChild(bannerNode);
});

function setLoadingScreen(bool, cb) {
    if(bool)
        MNGScriptPublic.loadingScreen.classList.add('active');
    else {
        MNGScriptPublic.loadingScreen.classList.remove('active');
    }
    if(cb)
    setTimeout(function() {
        return cb();
    }, 500);
}

let imagesOnSite = 0;
let imagesLoaded = 0;

function clearContent() {
    imagesOnSite = 0;
    imagesLoaded = 0;
    removeInjectedScripts();
    removeInjectedStyles();
}

function urlExists(url) {
    var http = new XMLHttpRequest();
    http.open('HEAD', url, false);
    http.send();
    return http.status!=404;
}

function loadBlog(name) {
    if(urlExists("site_content/" + name + "/min.json")) {
        loadStyle('blog.css');
        const script1 = loadScript('blog.js');
        script1.onload = function() {
            script(name);
        };
    } else {
        loadHome();
    }
}

function loadHome() {
    loadScript('script.js');
    loadStyle('style.css');
}

function loadScript(src) {
    const script = document.createElement('script');
    script.classList.add("autoinjected-script");
    script.src = src;
    script.type = "text/javascript";
    MNGScriptPublic.headElement.appendChild(script);
    return script;
}

function removeInjectedScripts() {
    const scripts = MNGScriptPublic.headElement.getElementsByClassName('autoinjected-script');
    if(unload) unload();
    script = null;
    unload = null;
    Array.from(scripts).forEach(function(script) {
        MNGScriptPublic.headElement.removeChild(script);
    });
}

function loadStyle(src) {
    const style = document.createElement('link');
    style.classList.add('autoinjected-style');
    style.href = src;
    style.rel = 'stylesheet';
    MNGScriptPublic.headElement.appendChild(style);
}

function removeInjectedStyles() {
    const styles = MNGScriptPublic.headElement.getElementsByClassName('autoinjected-style');
    Array.from(styles).forEach(function(style) {
        MNGScriptPublic.headElement.removeChild(style);
    });
}

function _fetch(src, cb) {
    const ajax = new XMLHttpRequest();
    ajax.addEventListener('load', function(event) {
        return cb(ajax.responseText);
    });

    ajax.open('GET', src);
    ajax.send();
}

function loadBlogStringList(cb) {
    const http = new XMLHttpRequest();
    http.addEventListener('load', function(event) {
        return cb(http.responseText);
    });
    http.open('GET', "./site_content/blogs.json");
    http.send();
}