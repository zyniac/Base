(script = function() {
    const AUTHORPOST = "Written by ";
    let firstPost = true;

    const mainContent = document.getElementById('main-content');

    function createDiv(c) {
        const div = document.createElement('div');
        div.classList.add(c);
        return div;
    }

    function createElement(e, c) {
        const element = document.createElement(e);
        element.classList.add(c);
        return element;
    }

    function createMainPageNewestBlogPost(writtenby, title, description, picturePath, folder) {
        const itemMainNewest = createDiv('item-main-newest');
        itemMainNewest.dataset.place = folder;
        const imgBackground = createElement('img', 'image');
        imgBackground.addEventListener('load', imageLoadEvent, false);
        imagesOnSite++;
        imgBackground.src = picturePath;
        const divOverlay = createDiv('overlay');
        const divInfoBox = createDiv('info-box');
        const paraWrittenBy = createElement('p', 'writtenby');
        const hTitle = createElement('h2', 'title');
        const paraDescription = createElement('p', 'description');
        const divPlaceholder = createDiv('placeholder-1');
        const divPlaceholder2 = createDiv('placeholder-1');
        paraWrittenBy.innerText = AUTHORPOST + writtenby;
        hTitle.innerText = title;
        paraDescription.innerText = description;
        divInfoBox.appendChild(paraWrittenBy);
        divInfoBox.appendChild(hTitle);
        divInfoBox.appendChild(paraDescription);
        divOverlay.appendChild(divInfoBox);
        itemMainNewest.appendChild(imgBackground);
        itemMainNewest.appendChild(divOverlay);

        return [divPlaceholder, itemMainNewest, divPlaceholder2];
    }

    function appendElements(elem) {
        elem.forEach(function(e) {
            mainContent.appendChild(e);
        });
    }

    function createMainPageBlogPost(writtenby, title, description, picturePath, folder) {
        const itemMain = createDiv('item-main');
        const container = createDiv('container');
        container.dataset.place = folder;
        const image = createElement('img', 'image');
        image.addEventListener('load', imageLoadEvent, false);
        imagesOnSite++;
        image.src = picturePath;
        const overlay = createDiv('overlay');
        const infoBox = createDiv('info-box');
        const paraWrittenBy = createElement('p', 'writtenby');
        const hTitle = createElement('h2', 'title');
        const paraDescription = createElement('p', 'description');
        paraWrittenBy.innerText = AUTHORPOST + writtenby;
        hTitle.innerText = title;
        paraDescription.innerText = description;
        infoBox.appendChild(paraWrittenBy);
        infoBox.appendChild(hTitle);
        infoBox.appendChild(paraDescription);
        overlay.appendChild(infoBox);
        container.appendChild(image);
        container.appendChild(overlay);
        itemMain.appendChild(container);

        return itemMain;
    }

    function createMainItemSection() {
        const itemSection = createDiv('main-item-section');
        return itemSection;
    }

    function appendToMainItemSection(section, item1, item2=null) {
        section.appendChild(item1);
        if(item2)
            section.appendChild(item2);
    }

    function appendMainItemSection(section) {
        mainContent.appendChild(section);
    }

    let lastPost = null;

    function execMin(folder, cb) {
        _fetch("./site_content/" + folder + "/min.json", function(res) {
            const obj = JSON.parse(res);
            if(firstPost || obj["Special"]) {
                firstPost = false;
                return cb(createMainPageNewestBlogPost(obj["WrittenBy"], obj["Title"], obj["Description"], "./site_content/" + folder + "/" + obj["Picture"], folder));
            } else {
                return cb(createMainPageBlogPost(obj["WrittenBy"], obj["Title"], obj["Description"], "./site_content/" + folder + "/" + obj["Picture"], folder));
            }
        });
    }

    function imageLoadEvent() {
        imagesLoaded++;
        if(imagesLoaded === imagesOnSite)
            setLoadingScreen(false);
    }

    (function loadContent() {
        loadBlogStringList(function(blogJson) {
            const blogObj = JSON.parse(blogJson);
            (function loop(i=0) {
                execMin(blogObj[i], function(elem) {
                    if(!Array.isArray(elem)) {
                        if(lastPost) {
                            const section = createMainItemSection();
                            appendToMainItemSection(section, lastPost, elem);
                            appendMainItemSection(section);
                            lastPost = null;
                        } else lastPost = elem;
                    } else
                        appendElements(elem);
                    
                    if(i === blogObj.length-1) {
                        if(lastPost) {
                            const section = createMainItemSection();
                            appendToMainItemSection(section, lastPost);
                            appendMainItemSection(section);
                        }
                    }
                    if(i < blogObj.length - 1) {
                        loop(i + 1);
                    }
                });
            })();
        });
    })();
})();

function postClick(event) {
    if(event.target.dataset.place) {
        console.log(event.target.dataset.place);
        window.location = document.URL.split('?')[0] + "?" + event.target.dataset.place;
    }
}

document.addEventListener('click', postClick);

unload = function() {
    document.removeEventListener('click', postClick);
    postClick = undefined;
};