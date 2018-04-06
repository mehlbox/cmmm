var line	= { set:{id:"set"}, current:{id:"current"}, target:{id:"target"}, speed:{ coordinate:{}}, animation:{id:"current"} }
var area	= { }
var option	= { }
var server	= { protocol: "https", host: "service."+location.hostname, port: "443" }
var memory	= { }
var navSelect;
var errorCount;
var auth = {}

$(document).ready(function() {
	$("#popupLogin").popup("open");
});

$("#position_area")
.on("mousedown", function(e) {
	delete option.loaded;
	var x, y, x_rel, y_rel, x_init, y_init;
	$("#infoBoxCurrent").hide();
	$("#infoBoxSet").show();
	e.stopPropagation();
	e.preventDefault();
	x = e.pageX;
	y = e.pageY;
	//x = x - vwTOpx(4); // distance to left
	y = y - emTOpx(3); // distance to top
	x = area.dimension.x - (x / $("#svg").width()  * area.dimension.x);
	y = area.dimension.y - (y / $("#svg").height() * area.dimension.y);

	if ($("#fine option:selected").prop("value") == "fine") {
		x_init = x;
		y_init = y;
	} else {
		check_set(x,y);
		draw(line.set);
	}

	$(document).on("mousemove", "#position_area", function (e) {
		e.stopPropagation();
		e.preventDefault();
		x = e.pageX;
		y = e.pageY;
		//x = x - vwTOpx(4);  // distance to left
		y = y - emTOpx(3); // distance to top
		x = area.dimension.x - (x / $("#svg").width()  * area.dimension.x);
		y = area.dimension.y - (y / $("#svg").height() * area.dimension.y);

		if ($("#fine option:selected").prop("value") == "fine") {
			x_rel = (x - x_init)/20;
			y_rel = (y - y_init)/20;
			if (x_rel >  2) x_rel =  2;
			if (x_rel < -2) x_rel = -2;
			if (y_rel >  2) y_rel =  2;
			if (y_rel < -2) y_rel = -2;

			x_init = x;
			y_init = y;
			x = line.set.coordinate.x + x_rel;
			y = line.set.coordinate.y + y_rel;
		}
		check_set(x,y);
		draw(line.set);
	})
})
.on("touchstart", function(e) {
	delete option.loaded;
	var x, y, x_rel, y_rel, x_init, y_init;
	$("#infoBoxCurrent").hide();
	$("#infoBoxSet").show();
	e.stopPropagation();
	e.preventDefault();
	x = e.originalEvent.touches[0].pageX;
	y = e.originalEvent.touches[0].pageY;
	//x = x - vwTOpx(4); // distance to left
	y = y - emTOpx(3); // distance to top
	x = area.dimension.x - (x / $("#svg").width()  * area.dimension.x);
	y = area.dimension.y - (y / $("#svg").height() * area.dimension.y);

	if ($("#fine option:selected").prop("value") == "fine") {
		x_init = x;
		y_init = y;
	} else {
		check_set(x,y);
		draw(line.set);
	}

	$(document).on("touchmove", "#position_area", function(e) {
		e.stopPropagation();
		e.preventDefault();
		x = e.originalEvent.touches[0].pageX;
		y = e.originalEvent.touches[0].pageY;
		//x = x - vwTOpx(4); // distance to left
		y = y - emTOpx(3); // distance to top
		x = area.dimension.x - (x / $("#svg").width()  * area.dimension.x);
		y = area.dimension.y - (y / $("#svg").height() * area.dimension.y);

		if ($("#fine option:selected").prop("value") == "fine") {
			x_rel = (x - x_init)/50;
			y_rel = (y - y_init)/50;
			if (x_rel >  2) x_rel =  2;
			if (x_rel < -2) x_rel = -2;
			if (y_rel >  2) y_rel =  2;
			if (y_rel < -2) y_rel = -2;

			x_init = x;
			y_init = y;
			x = line.set.coordinate.x + x_rel;
			y = line.set.coordinate.y + y_rel;
		}
		check_set(x,y);
		draw(line.set);
	});
});

$(document)
.on("change","#directCoordinateX", function (e) {
	line.set.coordinate.x = e.target.value*10;
	draw(line.set);
	$(this).blur();
})
.on("change","#directCoordinateY", function (e) {
	line.set.coordinate.y = e.target.value*10;
	draw(line.set);
	$(this).blur();
})
.on("change","#fine", function (e) {
	if ($("#fine option:selected").prop("value") != "fine") {
		$("#infoBoxCurrent").show();
		$("#infoBoxSet").hide();
	} else {
		$("#infoBoxCurrent").hide();
		$("#infoBoxSet").show();
	}
})
.on("change","#memIndexName", function (e) {
        $(this).blur();
})
.on("change","#memIndexSlider", function (e) {
	var tempIndex, tempName;
	tempIndex = $("#memIndexSlider").val();
	if ($.isEmptyObject(memory[tempIndex])) {
		tempName = "- leer -";
		$("#memIndexName").attr("placeholder",tempName)
		$("#memIndex").attr("onclick","callSlot("+tempIndex+");")
		$("#memIndexX").html(0);
		$("#memIndexY").html(0);
		$("#memIndexName").val("");
	} else {
		tempName = memory[tempIndex].name;
		$("#memIndexName").val(tempName);
		$("#memIndexName").attr("placeholder","")
		$("#memIndex").attr("onclick","callSlot("+tempIndex+");")
		$("#memIndexX").html(parseInt((memory[tempIndex].x/10)+0.5));
		$("#memIndexY").html(parseInt((memory[tempIndex].y/10)+0.5));
		if (navSelect == "load") {
			line.set.coordinate.x = memory[tempIndex].x;
			line.set.coordinate.y = memory[tempIndex].y;
			draw(line.set);
		}
	}
	if (tempName == "- ohne Bezeichnung -") {
		$("#memIndexName").attr("placeholder",tempName)
		$("#memIndexName").val("");
	}
	$("#memIndex").html(tempName);
	$(this).blur();
})

$("#emergencyStop").click( function() {
	var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "show", {
                        text: "senden",
                        textVisible: true
                });
        }, 500);
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/emergencyStop",
		timeout: 5000
	})
	.done(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "hide" );
		line.speed.lengh.front = 0;
		line.speed.lengh.back  = 0;
	})
	.fail(function(e) {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: e.responseText,
			textVisible: true,
			textonly: true
		})
	})
});

$("#stop").click( function() {
	var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "senden", {
                        text: "laden",
                        textVisible: true
                });
        }, 500);
	var x, y
	x = (Math.pow(line.current.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(line.current.lengh.back,2))/(2*area.dimension.x);
	y = Math.sqrt(Math.pow(line.current.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
	line.current.coordinate.x = x;
	line.current.coordinate.y = area.dimension.y - y;
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/target",
		timeout: 5000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(line.current.coordinate)
	})
	.done(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "hide" );
		line.set.coordinate = Object.assign(line.set.coordinate, line.current.coordinate);
		draw(line.set);
		delete option.loaded;
		$("#loaded").hide();
	})
	.fail(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#send").click( function() {
	var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "senden", {
                        text: "laden",
                        textVisible: true
                });
        }, 500);
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/target",
		timeout: 2000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(line.set.coordinate)
	})
	.done(function(response) {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#memLoad").click( function() {
	navSelect = "load";
	$("#memIndexName").addClass( "ui-state-disabled" )
	$(".indexListTop").css({"border-top": "0.2em solid #cce6b3"});
});

$("#memSave").click( function() {
	navSelect = "save";
	$("#memIndexName").removeClass( "ui-state-disabled" )
	$(".indexListTop").css({"border-top": "0.2em solid #ffccb3"});
});

$("#reloadPage").click( function() {
	location.reload();
});

$("#loginSubmit").click (function() {
	auth.un = $("#un").val();
	auth.pw = $("#pw").val();
	auth.string = btoa(auth.un + ":" + auth.pw);
	auth.pw = "";
	$("#popupLogin").popup("close");
	afterLogin();
});

$(document).mouseup(function(e) {
    $(document).off("mousemove", "#position_area");
		if ($("#fine option:selected").prop("value") != "fine") {
			$("#infoBoxCurrent").show();
			$("#infoBoxSet").hide();
		}
});

$(document).touchend(function() {
    $(document).off("touchmove", "#position_area");
		if ($("#fine option:selected").prop("value") != "fine") {
			$("#infoBoxCurrent").show();
			$("#infoBoxSet").hide();
		}
});

$('form').on('submit', function(e) {
	e.preventDefault();
});

window.oncontextmenu = function(event) {
     event.preventDefault();
     event.stopPropagation();
     return false;
};
