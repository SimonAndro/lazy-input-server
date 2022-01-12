$(function () {

    /**
     * Mouse handling
     */
    var targetDirectionX, targetDirectionY;

    $('#play-ground').on('touchend vmouseup', function (event) {
        ajaxRequest("/mouse-stop");
    });

    $("#id-keyboard").on("focusout", function () {
        $("#key-holder").html("Keyboard");
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



    /**
     * Keyboard handling
     */
    $("#id-keyboard").on("input", function (event) {

        // console.log(event,$("#id-keyboard").val())
        var input = $("#id-keyboard").val();

        if (input.trim().length == 0) {
            input = "Space";
        } else {

            $("#key-holder").html(input);
        }
        $("#id-keyboard").val(""); // clear input
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
    $("#id-keyboard").trigger("focus");
}

//.,l,l;;;;;;;;;;;;;;;;
const Keyboard = window.SimpleKeyboard.default;

const myKeyboard = new Keyboard({
  onChange: input => onChange(input),
  onKeyPress: button => onKeyPress(button)
});


function onChange(input) {
  document.querySelector(".input").value = input;
  console.log("Input changed", input);
}


function onKeyPress(button) {
  console.log("Button pressed", button);
}