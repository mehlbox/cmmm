var line	= { set:{id:"set"}, current:{id:"current"}, target:{id:"target"}, speed:{ coordinate:{}}, animation:{id:"current"} }
var area	= { }
var option	= { }
var server	= { protocol: "http", host: location.hostname, port: "82" }
//var server	= { protocol: "http", host: "192.168.10.36", port: "80" }
var memory	= { }
var navSelect;
var errorCount;

$(document).ready(function() {
	adminCheck();
	$.mobile.loading( "show", {
		text: "verbinden",
		textVisible: true
	});
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/all",
	//cache: false,
	timeout: 5000
	})
	.done(function(data) {
		area = Object.assign(area, data.area);
		line.current 	 = Object.assign(line.current, 	 data.line.current);
		line.target  	 = Object.assign(line.target,    data.line.target );
		//line.speed   	 = Object.assign(line.speed,  	 data.line.speed  );
		line.animation = Object.assign(line.animation, data.line.current);
		line.speed.lengh = {}
		line.speed.lengh.front = 0;
		line.speed.lengh.back  = 0;
			var x, y
			x = (Math.pow(line.current.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(line.current.lengh.back,2))/(2*area.dimension.x);
			y = Math.sqrt(Math.pow(line.current.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
			line.current.coordinate = {};
			line.current.coordinate.x = x;
			line.current.coordinate.y = area.dimension.y - y;
			line.set.coordinate = {};
			line.set.coordinate = Object.assign(line.set.coordinate, line.current.coordinate);
		option = Object.assign(option, data.option);
		set_area();
		set_option();
		set_lineLengh();
		draw_border();
		draw(line.set);
		draw(line.current);
		draw(line.target);
		animation();
		setTimeout("timeloop()",2000);
		$.mobile.loading("hide");
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "keine Verbindung",
			textVisible: true,
			textonly: true
		});
		setTimeout("location.reload()",60000);
	})
});

$("#position_area")
.on("mousedown", function(e) {
	var x, y, x_rel, y_rel, x_init, y_init;
	$("#infoBoxCurrent").hide();
	$("#infoBoxSet").show();
	e.stopPropagation();
	e.preventDefault();
	x = e.pageX;
	y = e.pageY;
	x = x - vwTOpx(4); // distance to left
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
		x = x - vwTOpx(4);  // distance to left
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
	var x, y, x_rel, y_rel, x_init, y_init;
	$("#infoBoxCurrent").hide();
	$("#infoBoxSet").show();
	e.stopPropagation();
	e.preventDefault();
	x = e.originalEvent.touches[0].pageX;
	y = e.originalEvent.touches[0].pageY;
	x = x - vwTOpx(4); // distance to left
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
		x = x - vwTOpx(4); // distance to left
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
})
.on("change","#directCoordinateY", function (e) {
	line.set.coordinate.y = e.target.value*10;
	draw(line.set);
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
.on("change","#memIndexSlider", function (e) {
	var tempIndex, tempName;
	tempIndex = $("#memIndexSlider").val();
	if ($.isEmptyObject(memory[tempIndex])) {
		memory[tempIndex] = {};
		memory[tempIndex].name = "- leer -";
	}
	if (!$.isEmptyObject(memory[tempIndex].name)) tempName = memory[tempIndex].name; else tempName = "";
	$("#memIndex").html("Speicherplatz: "+tempIndex+"&emsp;&emsp;"+tempName);
	$("#memIndex").attr("onclick","callSlot("+tempIndex+");")
	$("#memIndexX").html(parseInt((memory[tempIndex].x/10)+0.5));
	$("#memIndexY").html(parseInt((memory[tempIndex].y/10)+0.5));
	if (navSelect == "load") {
		line.set.coordinate.x = memory[tempIndex].x;
		line.set.coordinate.y = memory[tempIndex].y;
		draw(line.set);
	}
})

$("#stop").click( function() {
	$.mobile.loading( "show", {
		text: "senden",
		textVisible: true
	});
	var x, y
	x = (Math.pow(line.current.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(line.current.lengh.back,2))/(2*area.dimension.x);
	y = Math.sqrt(Math.pow(line.current.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
	line.current.coordinate.x = x;
	line.current.coordinate.y = area.dimension.y - y;
	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/target",
	timeout: 5000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(line.current.coordinate)
	})
	.done(function() {
		$.mobile.loading( "hide" );
		line.set.coordinate = Object.assign(line.set.coordinate, line.current.coordinate);
		draw(line.set);
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#send").click( function() {
	$.mobile.loading( "show", {
		text: "senden",
		textVisible: true
	});
	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/target",
	timeout: 2000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(line.set.coordinate)
	})
	.done(function(response) {
		$.mobile.loading( "hide" );
		// get border
		//area = Object.assign(area, JSON.parse(response));
		//set_area();
		//draw_border();
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#memLoad").click( function() {
	navSelect = "load";
	$(".indexListTop").css({"border-top": "0.2em solid #ccffcc"});
});

$("#memSave").click( function() {
	navSelect = "save";
	$(".indexListTop").css({"border-top": "0.2em solid #ffcccc"});
});

$("#memLoad,#memSave").click( function() {
	if ($.isEmptyObject(memory)) {
		$.mobile.loading( "show", {
			text: "laden",
			textVisible: true
		})
		$.ajax({
		url: server.protocol+"://"+server.host+":"+server.port+"/readMemory",
		//cache: false,
		timeout: 2000
		})
		.done(function(response) {
			$.mobile.loading( "hide" );
			memory = Object.assign(memory, response);
			create_memIndexList();
		})
		.fail(function() {
			$.mobile.loading( "show", {
				text: "fehlgeschlagen",
				textVisible: true,
				textonly: true
			})
		})

	}
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
