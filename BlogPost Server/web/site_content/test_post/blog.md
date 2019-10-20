#How to write a Hello World Website

(This is not a real tutorial. It is only for testing)

First you need to make two files in one folder of your choice. The first should be saved as ``index.html`` and the second as ``style.css``.

<br>
HTML
```html
<!DOCTYPE html>
<html>
    <head>
        <link rel="stylesheet" href="style.css">
    </head>

    <body>
        <div>
            <p class="text">Hello World<p>
        </div>
    </body>
</html>
```
<br>
CSS
```css
div > p.text {
    /* scheme */
    /* Setting: Value */
    font-family: Arial; /* Setting font-type to Arial */
    font-size: 20px;
}
```
<br>
Now open the ``index.html`` with your browser.
This should give you something like this:
<p style="font-family: Arial; font-size: 20px; margin: 0 0 20px 0; background: #222222; padding: 0 0 0 10px; user-select: text;">Hello World<p>

And thats it! You have created your first Website from scratch.
If you'd like to read some intermediate tutorials, click on one of these links:

- [HTML Canvas Tutorial](https://google.de)