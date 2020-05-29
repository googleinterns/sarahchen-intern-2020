/*
    Copyright 2020 Google LLC

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
const time_id = "REFRESHED";
const dataSources = ["SPAM", "PLAY_COUNTRY", "APP_COUNTRY_PUBLISH_TIME", 
    "QUERY_CATEGORY_SOURCE", "UNIFIED_USER_DATA_SOURCE"];
const systemSources = ["CHIPPER", "CHIPPER_GDPR"];
const dates = ["5/19/2020", "5/18/2020", "5/17/2020", "5/16/2020", "5/15/2020", 
    "5/14/2020", "5/13/2020"]
let showing = null;
systemSources.forEach(populate_info_buttons);


window.onload = () => {
    let xhr = new XMLHttpRequest();
    xhr.open("GET", "http://localhost:8000/", true);
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            let time = document.getElementById(time_id);
            time.innerHTML = xhr.responseText;
        }
    }
    xhr.send();
}

function populate_info_buttons (item, index) {
    document.getElementById(item).onclick = function(e) {
        // prevent an item to have more than one event listener
        e.stopPropagation();
        document.addEventListener("click", more_info(item));
    }
}

function populate_command(system, remove) {
    let lst = []
    for (let i = 0; i < dates.length; ++i) {
        for (let j = 0; j < dataSources.length; ++j) {
            lst.push(dataSources[j]+ "%" + dates[i] + "%" + system);
        }
    }
    for (let i = 0; i < lst.length; ++i) {
        if (remove) {
            document.getElementById(lst[i]).onclick = function(e) {
                e.stopPropagation();
                document.removeEventListener("click", print_command(lst[i]));
            }
        }
        else {
            document.getElementById(lst[i]).onclick = function(e) {
                e.stopPropagation();
                document.addEventListener("click", print_command(lst[i]));
            }
        }
    }
}

function print_command(clicked) {
    let xhr = new XMLHttpRequest();
    let info = clicked.split("%");
    let date = info[1].replace(new RegExp("/", "g"), "-");
    let url = "http://localhost:8000/" + info[2] + "/" + info[0] + "/" + date;
    xhr.open("GET", url, true);
    xhr.onreadystatechange = () => {
        // state of 4 is DONE
        if (xhr.readyState === 4) {
            alert(xhr.responseText);
        }
    }
    xhr.send();
}

function more_info(system) {
    let xhr = new XMLHttpRequest();
    let url = "http://localhost:8000/" + system;
    if (showing !== null) {
        // Hide text when user clicks the same system again
        populate_command(system, true);
        let text = document.getElementById(showing);
        text.remove();
        if (showing === system) {
            showing = null;
            return;
        }
    }
    xhr.open("GET", url, true);
    xhr.onreadystatechange = () => {
        if (xhr.readyState === 4) {
            document.body.innerHTML += xhr.responseText;
            populate_command(system, false);
        }
    }
    xhr.send();
    showing = system;
}

function update_button(btnText, moreText) {
    for (let i = 0; i < systemSources.length; ++i) {
        if(systemSources[i] + "_TEXT" === moreText.id) {
            continue;
        }
        let tmpMoreText = document.getElementById(systemSources[i] + "_TEXT");
        tmpMoreText.style.display = "none";
    }
}