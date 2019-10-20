(script = function(foldername) {
    const MAX_OTHER_POSTS = 5;

    const mainContent = document.getElementById('main-content');
    imagesOnSite++;
    (function loadMarkdown() {
        const path = "site_content/" + foldername + "/";
        _fetch(path + "min.json", function(minjsonraw) {
            const minObj = JSON.parse(minjsonraw);
            const blogFName = minObj["BlogSrc"];
            _fetch(path + blogFName, function(blogSrc) {
                createBlogPage();
                loadOtherPosts();
                document.addEventListener('DOMContentLoaded', function() {
                    setLoadingScreen(false);
                });
                wsPath.innerText = minObj["Title"];

                const blogText = document.getElementById('blog-text');
                const script = loadScript("https://cdnjs.cloudflare.com/ajax/libs/showdown/1.9.0/showdown.min.js");
                script.onload = function() {
                    showdown.setOption('tables', true);
                    showdown.setOption('tasklists', true);
                    showdown.setOption('strikethrough', true);
                    showdown.setOption('emoji', true);
                    showdown.setOption('simpleLineBreaks', false);
                    showdown.setOption('openLinksInNewWindow', true);
                    //showdown.setOption('disableForced4SpacesIndentedSublists', true);
                    const converter = new showdown.Converter(),
                    text = blogSrc,
                    html = converter.makeHtml(text);
                    blogText.innerHTML = html;

                    (function loadSyntaxHighlighter() {
                        loadScript('prism/prism.js');
                        loadStyle('prism/tomorrownight.css');
                        setTimeout(imageLoadEvent, 500);
                    })();
                };
            });
        });
    })();

    let wsPath = null;
    let wsOtherPosts = null;

    function createBlogPage() {
        const placeholder1 = document.createElement('div');
        placeholder1.classList.add('placeholder-1');
        const header = document.createElement('div');
        header.classList.add('header');
        const headerTitle = document.createElement('p');
        headerTitle.classList.add('title');
        wsPath = headerTitle;
        const placeholder2 = document.createElement('div');
        placeholder2.classList.add('placeholder-2');
        const blogBlock = document.createElement('div');
        blogBlock.classList.add('blog-block');
        const blogHolder = document.createElement('div');
        blogHolder.classList.add('blog-holder');
        const blogText = document.createElement('div');
        blogText.id = "blog-text";
        const placeholder3 = document.createElement('div');
        placeholder3.classList.add('placeholder-1');
        blogText.classList.add('blog-text');

        const otherPosts = document.createElement('div');
        otherPosts.classList.add('other-posts');
        wsOtherPosts = otherPosts;
        mainContent.appendChild(placeholder1);
        blogHolder.appendChild(blogText);
        blogBlock.appendChild(blogHolder);
        header.appendChild(headerTitle);
        mainContent.appendChild(header);
        mainContent.appendChild(placeholder2);
        mainContent.appendChild(blogBlock);
        mainContent.appendChild(otherPosts);
        mainContent.appendChild(placeholder3);
    }

    function loadOtherPosts() {
        loadBlogStringList(function(listSrc) {
            const list = JSON.parse(listSrc);
            (function loop(i=0) {
                _fetch("site_content/" + list[i] + "/min.json", function(src) {
                    console.log(src);
                    const minObj = JSON.parse(src);
                    createAndAppendOPElement(list[i], minObj);
                    if(i < list.length - 1 && i < MAX_OTHER_POSTS)
                        loop(i + 1);
                });
            })();
        });
    }

    function imageLoadEvent() {
        imagesLoaded++;
        if(imagesLoaded === imagesOnSite)
            setLoadingScreen(false);
    }

    // Build Other Post Element
    function createAndAppendOPElement(folder, minObj) {
        const post = document.createElement('div');
        post.classList.add('post');
        post.dataset.place = folder;
        post.addEventListener('click', function() {
            window.location = document.URL.split('?')[0] + "?" + event.target.dataset.place;
        });
        const imageHolder = document.createElement('div');
        imageHolder.classList.add('image-holder');
        const image = document.createElement('img');
        imagesOnSite++;
        image.addEventListener('load', imageLoadEvent, false);
        const overlay = document.createElement('div');
        overlay.classList.add('overlay');
        const overlayInfoBox = document.createElement('div');
        overlayInfoBox.classList.add('info-box');
        const overlayTitle = document.createElement('p');
        overlayTitle.classList.add('title');
        overlayTitle.innerText = minObj["Title"];
        const overlayDescription = document.createElement('p');
        overlayDescription.classList.add('description');
        overlayDescription.innerText = minObj["Description"];
        const overlayWrittenBy = document.createElement('p');
        overlayWrittenBy.classList.add('written-by');
        overlayWrittenBy.innerText = minObj["WrittenBy"];
        overlayInfoBox.appendChild(overlayWrittenBy);
        overlayInfoBox.appendChild(overlayTitle);
        overlayInfoBox.appendChild(overlayDescription);
        overlay.appendChild(overlayInfoBox);
        image.src = "site_content/" + folder + "/" + minObj["Picture"];
        image.classList.add('image');
        imageHolder.appendChild(image);
        imageHolder.appendChild(overlay);
        post.appendChild(imageHolder);
        wsOtherPosts.appendChild(post);
    }
});