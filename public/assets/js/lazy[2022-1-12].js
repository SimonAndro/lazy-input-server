$(function () {

    var touchStartX=0, touchStartY=0;
    var touchEndX=0, touchEndY=0;
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

    var timeout = null;
    var count = 1;
    $('#play-ground').on('touchmove vmousemove', function (event) {
        console.log('touch move: ', event.pageX, event.pageY);
        //$("#move").html(event.pageX+"->>"+event.pageY);

        if(((0/event.pageX) != NaN) && ((0/event.pageY) != NaN))
        {
            // moveX = Number(moveX) +  Number(event.pageX || 0);
            // moveY = Number(moveY) + Number(event.pageY || 0);
            // count ++;

            moveX =  Number(event.pageX || 0) - Number(moveX);
            moveY =  Number(event.pageY || 0) - Number(moveY);
           
        }else{
            return false;
        }

        if(startTouch)
        {
            startTouch = false;
            touchStartX = moveX;
            touchStartY = moveY;

            $("#start").html(event.pageX + "->>" + event.pageY);
        }

        $("#res").after( moveX+"->"+moveY+"<br>");

        // if (timeout == null) {
        //     timeout = setTimeout(function () {
        //         timeout = null;                

                // targetDirectionX = parseInt(moveX)/count - touchStartX;
        
                // targetDirectionY = parseInt(moveY)/count - touchStartY;

                // count = 1;

                targetDirectionX = moveX;
        
                targetDirectionY = moveY;
                        
                $("#move").html(targetDirectionX + "->>" + targetDirectionY);

                //send information to server
                $.ajax({
                    url: "/mouse-move",
                    type: "GET",
                    data: {
                        x: targetDirectionX < 0 ? -1 : 1,
                        y: targetDirectionY < 0 ? -1 : 1,
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

                moveY = [];
                moveX = [];

        //     }, 100);
        // }

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