## Preparation
    1.  Clone the repository 
        git clone https://github.com/googleinterns/sarahchen-intern-2020.git
    2.  Navigate into the "hologram_dashboard" folder
        cd hologram_dashboard

## Server 
Follow the following steps to run the server 

    1.  Install bazel by following the steps of 
        https://docs.bazel.build/versions/3.2.0/install.html
        for your specific system eg. ubuntu
    2.  Build the server from the hologram_dashboard folder
        bazel build //server:server
    3.  Run the server to start listening on port 8000
        bazel run //server:server

## Extension
Follow the following steps to upload the extension onto chrome

    1.  Navigate to chrome://extensions from a chrome browser
    2.  Enable developer's mode by clicking the developer mode option on the
        top right
        https://screenshot.googleplex.com/C9o8CfEfG3P
    3.  Click "load unpacked" to load "hologram_dashboard/front-end/" then the
        extension should show on your dashboard with a google logo
        https://screenshot.googleplex.com/ZHXFaaQtY8W
