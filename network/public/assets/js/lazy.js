$(function () {

    /**
     * Mouse handling
     */
    var targetDirectionX, targetDirectionY;

    $('#play-ground').on('touchend vmouseup', function (event) {
        ajaxRequest("/mouse-stop");
    });

    //Enable swiping...
    $("#play-ground").swipe({
        swipeStatus: function (event, phase, direction, distance, duration, fingers, fingerData, currentDirection) {

            targetDirectionY = 0;
            targetDirectionX = 0;
            if (currentDirection == "up") {

                targetDirectionY = -1;
            }

            if (currentDirection == "right") {

                targetDirectionX = 1;
            }

            if (currentDirection == "down") {

                targetDirectionY = 1;
            }

            if (currentDirection == "left") {

                targetDirectionX = -1;
            }

            $("#move").html(targetDirectionX + "->>" + targetDirectionY);

            //send information to server         
            var data = {
                x: targetDirectionX,
                y: targetDirectionY,
                end: true
            };
            ajaxRequest("/mouse-move", "GET", data);
        },
        tap: function (event, target) {
            //$("#test").html("tap");

            if (!targetDirectionX && !targetDirectionY) {
                ajaxRequest("/mouse-left");
            }

        },
        doubleTap: function (event, target) {
            //$("#test").html("double tap");

            ajaxRequest("/double-tap");
        },
        longTap: function (event, target) {
            //$("#test").html("long tap");

            ajaxRequest("/long-tap");
        },
        threshold: 200,
        maxTimeThreshold: 10000,
        fingers: 'all'
    });

    var scrollBtnTop = 150;
    var scrollDefault = 150
    var repeat = 1;

    $("#scroll-btn").swipe({
        swipeStatus: function (event, phase, direction, distance, duration, fingers, fingerData, currentDirection) {

            if (phase == "end" || phase == "cancel") { // snap back
                scrollBtnTop = scrollDefault;
                repeat = 1;

                ajaxRequest("/mouse-stop");
            }

            if (currentDirection == "up") {
                if (scrollBtnTop > scrollDefault - 50)
                    scrollBtnTop = scrollBtnTop - repeat++;

                ajaxRequest("/scroll-up");
            }


            if (currentDirection == "down") {
                if (scrollBtnTop < scrollDefault + 50)
                    scrollBtnTop = scrollBtnTop + repeat++;

                ajaxRequest("/scroll-down");
            }

            $("#scroll-btn-ctn").css({
                "top": scrollBtnTop + "px"
            });
        },
        tap: function (event, target) {
            // $("#test").html("tap");

            if (repeat == 1) {
                ajaxRequest("/wheel-down");
            }

        },
        threshold: 200,
        maxTimeThreshold: 5000,
        fingers: 'all'
    });

    $("#left-click").on("click", function () {
        ajaxRequest("/mouse-left");
    });

    $("#right-click").on("click", function () {
        ajaxRequest("/mouse-right");
    });
}());


function ajaxRequest(url, type, data) {

    type = type || "GET";
    data = data || {};

    $.ajax({
        url: url,
        type: "GET",
        data: data,
        success: function (res) {
            console.log(res);
            $("#res").html(res);
        },
        error: function (res) {
            console.log(res);
            $("#res").html(res);
        }
    });
}

function supportOptionChange(t) {
    var o = $(t);
    if (o.val() == "wechat") {
        $("#support-code").html(`<img style="width:100%; height:auto;" src="" alt="Wechat">`);
        $("#support-code img").attr("src", "assets/img/coffee.jpg");
    } else {
        $("#support-code").html("");
    }
}

function showKeyboard() {
    if ($(".keyboard-container:visible").length) {
        $(".keyboard-container").slideUp();
        $("#key-holder").html("Start typing...");
    } else {
        $(".keyboard-container").slideDown();
    }
}

//keyboard handling
const Keyboard = window.SimpleKeyboard.default;

const myKeyboard = new Keyboard({
    onChange: input => onChange(input),
    onKeyPress: button => onKeyPress(button)
});


function onChange(input) {
    document.querySelector(".input").value = input;
    myKeyboard.clearInput();

    console.log("Input changed", input);

}

var isShift = 0;
var virtualKey = 00;

var keytoVirtualCode = {
    "0": "30",
    "1": "31",
    "2": "32",
    "3": "33",
    "4": "34",
    "5": "35",
    "6": "36",
    "7": "37",
    "8": "38",
    "9": "39",
    "`": "C0",
    "-": "BD",
    "=": "BB",
    "{bksp}": "08",
    "{tab}": "09",
    "q": "51",
    "w": "57",
    "e": "45",
    "r": "52",
    "t": "54",
    "y": "59",
    "u": "55",
    "i": "49",
    "o": "4f",
    "p": "50",
    "[": "DB",
    "]": "DD",
    "\\": "DC",
    "{lock}": "14",
    "a": "41",
    "s": "53",
    "d": "44",
    "f": "46",
    "g": "47",
    "h": "48",
    "j": "4a",
    "k": "4b",
    "l": "4c",
    ";": "BA",
    "'": "DE",
    "{enter}": "0D",
    "{shift}": "10",
    "z": "5a",
    "x": "58",
    "c": "43",
    "v": "56",
    "b": "42",
    "n": "4e",
    "m": "4d",
    ",": "BC",
    ".": "BE",
    "/": "BF",
    "{space}": "20",

    /**
     * after pressing shift
     */
    "~": "C0",
    "!": "30",
    "@": "31",
    "#": "32",
    "$": "33",
    "%": "34",
    "^": "35",
    "&": "36",
    "*": "37",
    "(": "38",
    ")": "39",
    "_": "BD",
    "+": "BB",
    "Q": "51",
    "W": "57",
    "E": "45",
    "R": "52",
    "T": "54",
    "Y": "59",
    "U": "55",
    "I": "49",
    "O": "4f",
    "P": "50",
    "{": "DB",
    "}": "DD",
    "|": "DC",
    "A": "41",
    "S": "53",
    "D": "44",
    "F": "46",
    "G": "47",
    "H": "48",
    "J": "4a",
    "K": "4b",
    "L": "4c",
    ":": "BA",
    "\"": "DE",
    "Z": "5a",
    "X": "58",
    "C": "43",
    "V": "56",
    "B": "42",
    "N": "4e",
    "M": "4d",
    "<": "BC",
    ">": "BE",
    "?": "BF",
};

function onKeyPress(button) {
    console.log("Button pressed", button);

    $("#key-holder").html(button);

    /**
     * handle the shift and caps lock buttons
     */
    if (button === "{shift}") {
        isShift = (isShift == 0) ? 1 : 0;
        handleShift();
    }

    if (button === "{lock}") {
        handleShift();
    }

    virtualKey = keytoVirtualCode[button];
    if (!virtualKey) {
        return false;
    }

    if (button !== "{shift}") //shift is only transmitted as state
    {
        ajaxRequest("/keyboard", "GET", {
            k: virtualKey,
            s: isShift,
            end: true
        });
    }

}


function handleShift() {
    let currentLayout = myKeyboard.options.layoutName;
    let shiftToggle = currentLayout === "default" ? "shift" : "default";

    myKeyboard.setOptions({
        layoutName: shiftToggle
    });

}