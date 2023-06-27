function change() {
    var text = document.getElementById("beforeText");
    var image = document.getElementById("beforeImage");
    text.textContent="After";
    image.setAttribute("src", "gr-large.jpg");
}

document.addEventListener("DOMContentLoaded", function() {
    setTimeout("change()", 1000)
});