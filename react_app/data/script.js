// To call the navbar and footer htmls into the index html
const nav = document.querySelector('.navbar')
fetch('/navbar.html')
.then(res=>res.text())
.then(data=>{
    nav.innerHTML=data
    const parser = new DOMParser()
    const doc = parser.parseFromString(data, 'text/html')
    eval(doc.querySelector('script').textContent)
})

const footer = document.querySelector('.footer')
fetch('/footer.html')
.then(res=>res.text())
.then(data=>{
    footer.innerHTML=data
    const parser = new DOMParser()
    const doc = parser.parseFromString(data, 'text/html')
    eval(doc.querySelector('script').textContent)
})

// ESP32 Update web page without refresh
setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getData();
}, 2000); //2000mSeconds update rate

function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
        document.getElementById("TempValue").innerHTML = this.responseText;
    }
    };
    xhttp.open("GET", "/temp/getTemp", true);
    xhttp.send();
}