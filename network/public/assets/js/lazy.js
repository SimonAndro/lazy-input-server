$(function () {

    var touchStartX, touchStartY;
    var touchEndX, touchEndY;
    var moveX, moveY;
    $('#play-ground').on('touchstart vmousedown', function(event){
        console.log('Touch start: ',event.pageX,event.pageY);
        $("#start").html(event.pageX+"->>"+event.pageY);

        touchStartX = event.pageX;
        touchStartY = event.pageY;
    })
    
    $('#play-ground').on('touchmove vmousemove', function(event){
        console.log('touch move: ',event.pageX,event.pageY);
        $("#move").html(event.pageX+"->>"+event.pageY);

        moveX = event.pageX;
        moveY = event.pageY;
    })
    
    $('#play-ground').on('touchend vmouseup', function(event){
        $("#end").html(event.pageX+"->>"+event.pageY);

        touchEndX = event.pageX;
        touchEndY = event.pageY;
    });

}());