$(function () {

    var touchStartX, touchStartY;
    var touchEndX, touchEndY;
    var moveX, moveY;
    var targetDirectionX, targetDirectionY;
    $('#play-ground').on('touchstart vmousedown', function (event) {
        console.log('Touch start: ', event.pageX, event.pageY);
        $("#start").html(event.pageX + "->>" + event.pageY);

        touchStartX = event.pageX;
        touchStartY = event.pageY;
    })

    $('#play-ground').on('touchmove vmousemove', function (event) {
        console.log('touch move: ', event.pageX, event.pageY);
        //$("#move").html(event.pageX+"->>"+event.pageY);

        moveX = event.pageX;
        moveY = event.pageY;

        targetDirectionX = parseInt(touchStartX - moveX);
        targetDirectionY = parseInt(touchStartY - moveY);


        $("#move").html(targetDirectionX + "->>" + targetDirectionY);

        //send information to server
        /**
         * 00 -> mouse
         *      x y
         *      -->00 
         *         01 
         *         10
         *         11
         * 01 -> keyboard
         * 11 -> Scroll
         */
        //mouse encode
        var encode = "00";
        $.ajax({
            url: "/00",
            type: "GET",
            data: {
                x: targetDirectionX < 0 ? -1 : 1,
                y: targetDirectionY < 0 ? -1 : 1,
                end:true
            }
        });
    })

    $('#play-ground').on('touchend vmouseup', function (event) {
        $("#end").html(event.pageX + "->>" + event.pageY);

        touchEndX = event.pageX;
        touchEndY = event.pageY;
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

    // handle enter, backspace ...
    // $("#id-keyboard").on("keyup", function(event){
    //     if(event.key.toLowerCase)
    // });

}());

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