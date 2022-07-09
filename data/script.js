setInterval(function () {
  refreshValue("time");
}, 1000);

function refreshValue(elementid) {
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById(elementid).innerHTML = this.responseText;
    }
  };
  xhr.open("GET", "/" + elementid, true);
  xhr.send();
}

const showElement = (element, status) => (element.disabled = !status);

function toggleCheckbox(element) {
  console.log("toggleCheckbox: " + element.id);
  var xhr = new XMLHttpRequest();
  if (element.checked) {
    xhr.open("GET", "/set?element=" + element.id + "&value=1", true);
  } else {
    xhr.open("GET", "/set?element=" + element.id + "&value=0", true);
  }
  xhr.send();
}

function processingButton(element) {
  element.disabled = true;
  setTimeout(timerEnableButton, 1000, element);
  var xhr = new XMLHttpRequest();
  if (element.id == "button3") {
    xhr.open(
      "GET",
      "/set?element=" +
        element.id +
        "&value=" +
        Number(document.querySelector("#input_1").value),
      true
    );
  } else {
    xhr.open("GET", "/set?element=" + element.id, true);
  }
  xhr.send();
}

function timerEnableButton(element) {
  //console.log("timerEnableButton: " + element.id);
  element.disabled = false;
}
