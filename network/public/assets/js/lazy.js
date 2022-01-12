$(function () {

    var touchStartX = 0,
        touchStartY = 0;
    var touchEndX = 0,
        touchEndY = 0;
    var moveX = 0.0,
        moveY = 0.0;
    var targetDirectionX, targetDirectionY;

    startTouch = false;

    $('#play-ground').on('touchstart vmousedown', function (event) {
        console.log('Touch start: ', event.pageX, event.pageY);
        // $("#start").html(event.pageX + "->>" + event.pageY);

        startTouch = true;
        touchStartX = event.pageX;
        touchStartY = event.pageY;
    })


    $('#play-ground').on('touchend vmouseup', function (event) {
        $("#end").html(event.pageX + "->>" + event.pageY);

        touchEndX = event.pageX;
        touchEndY = event.pageY;

        $.ajax({
            url: "/mouse-stop",
            type: "GET",
            success: function (res) {
                console.log(res);
                $("#res").html(res);
            },
            error: function (res) {
                console.log(res);
                $("#res").html(res);
            }
        });

    });

    $("#id-keyboard").on("focusout", function () {
        $("#key-holder").html("Keyboard");
    });

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

    //Enable swiping...
    $("#play-ground").swipe({
        swipeStatus: function (event, phase, direction, distance, duration, fingers, fingerData, currentDirection) {
            var str = "<h4>Swipe Phase : " + phase + "<br/>";
            str += "Current direction: " + currentDirection + "<br/>";
            str += "Direction from inital touch: " + direction + "<br/>";
            str += "Distance from inital touch: " + distance + "<br/>";
            str += "Duration of swipe: " + duration + "<br/>";
            str += "Fingers used: " + fingers + "<br/></h4>";
            //Here we can check the:
            //phase : 'start', 'move', 'end', 'cancel'
            //direction : 'left', 'right', 'up', 'down'
            //distance : Distance finger is from initial touch point in px
            //duration : Length of swipe in MS
            //fingerCount : the number of fingers used
            if (phase != "cancel" && phase != "end") {
                if (duration < 5000)
                    str += "Under maxTimeThreshold.<h3>Swipe handler will be triggered if you release at this point.</h3>"
                else
                    str += "Over maxTimeThreshold. <h3>Swipe handler will be canceled if you release at this point.</h3>"
                if (distance < 200)
                    str += "Not yet reached threshold.  <h3>Swipe will be canceled if you release at this point.</h3>"
                else
                    str += "Threshold reached <h3>Swipe handler will be triggered if you release at this point.</h3>"
            }
            if (phase == "cancel")
                str += "<br/>Handler not triggered. <br/> One or both of the thresholds was not met "
            if (phase == "end")
                str += "<br/>Handler was triggered."
            $("#test").html(str);

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
            $.ajax({
                url: "/mouse-move",
                type: "GET",
                data: {
                    x: targetDirectionX ,
                    y: targetDirectionY ,
                    end: true
                },
                success: function (res) {
                    console.log(res);
                    $("#res").html(res);
                },
                error: function (res) {
                    console.log(res);
                    $("#res").html(res);
                }
            });
        },
        threshold: 200,
        maxTimeThreshold: 5000,
        fingers: 'all'
    });


}());


function swipeDirection(direction) {

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