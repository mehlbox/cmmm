$(document)
.on("change","#range_x_min", function (e) {
	area.range.x.min = $("#range_x_min").val()*10;
	draw_border();
})
.on("change","#range_x_max", function (e) {
	area.range.x.max = $("#range_x_max").val()*10;
	draw_border();
})
.on("change","#range_y_min", function (e) {
	area.range.y.min = $("#range_y_min").val()*10;
	draw_border();
})
.on("change","#range_y_max", function (e) {
	area.range.y.max = $("#range_y_max").val()*10;
	draw_border();
})
.on("change","#dimension_y", function (e) {
	area.dimension.y = $("#dimension_y").val()*10;
	draw_border();
})
.on("change","#dimension_x", function (e) {
	area.dimension.x = $("#dimension_x").val()*10;
	draw_border();
})
.on("change","#dimension_z", function (e) {
	area.dimension.z = $("#dimension_z").val()*10;
})
.on("change","#spool_front", function (e) {
	option.spool.front = $("#spool_front").val()*1000;
})
.on("change","#spool_back", function (e) {
	option.spool.back = $("#spool_back").val()*1000;
})
.on("change","#motorSpeedLimit", function (e) {
	option.motorSpeedLimit = $("#motorSpeedLimit").val();
})
.on("change","#gearRatio", function (e) {
	option.gearRatio = $("#gearRatio").val();
})
.on("change","#stepping", function (e) {
	$("#stepping").trigger("click");
})

$("#stepping").click( function() {
	var tempStepping = $("#stepping").val();
	if ( tempStepping <  1 ) tempStepping = 1;
	if ( tempStepping == 3 && option.stepping == 4 ) tempStepping = 2;
	if ( tempStepping == 3 && option.stepping == 2 ) tempStepping = 4;
	if ( tempStepping <  8 && option.stepping == 8 ) tempStepping = 4;
	if ( tempStepping >  4 && option.stepping == 4 ) tempStepping = 8;
	if ( tempStepping < 16 && option.stepping == 16) tempStepping = 8;
	if ( tempStepping >  8 && option.stepping == 8 ) tempStepping = 16;
	if ( tempStepping > 16 ) tempStepping = 16;
	option.stepping = tempStepping;
	$("#stepping").val(option.stepping);
});


$("#area_send").click( function() {
	$.mobile.loading( "show", {
		text: "senden",
		textVisible: true
	});
	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/area",
	timeout: 5000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(area)
	})
	.done(function() {
		$.mobile.loading( "hide" );
		set_area();
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#area_cancel").click( function() {
	getArea();
});

$("#option_send").click( function() { //needs spool calculation here
	$.mobile.loading( "show", {
		text: "senden",
		textVisible: true
	});
	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/option",
	timeout: 5000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(option)
	})
	.done(function(receive) {
		location.reload();
		//$.mobile.loading( "hide" );
		//set_option();
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
});

$("#option_cancel").click( function() {
	getOption();
});

$("#adminReboot").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/reboot",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminShutdown").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/shutdown",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminRestart").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/restart",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminKill").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/kill",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminCommand").click( function() {
	$("#adminOutput").html("");
});

$("#option_cancel").click( function() {
	getOption();
});

$("#adminI2Cscan").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/i2cScan",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminReadMemory").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/readMemory",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(JSON.stringify(data, null, 2));
	})
});

$("#adminClearMemory").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/clearMemory",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#adminFactoryDefault").click( function() {
  $.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/factorydefault",
	timeout: 4000
	})
	.always(function(data) {
		$("#adminOutput").html(data.responseText);
	})
});

$("#invert_front_left").click( function() {
	$.mobile.loading( "show", {
		text: "sending",
		textVisible: true
	});
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/invert1",
	timeout: 5000
	})
	.done(function() {
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
});

$("#invert_back_left").click( function() {
	$.mobile.loading( "show", {
		text: "sending",
		textVisible: true
	});
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/invert2",
	timeout: 5000
	})
	.done(function() {
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
});

$("#invert_front_right").click( function() {
	$.mobile.loading( "show", {
		text: "sending",
		textVisible: true
	});
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/invert3",
	timeout: 5000
	})
	.done(function() {
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
});

$("#invert_back_right").click( function() {
	$.mobile.loading( "show", {
		text: "sending",
		textVisible: true
	});
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/invert4",
	timeout: 5000
	})
	.done(function() {
		$.mobile.loading( "hide" );
	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "failed",
			textVisible: true,
			textonly: true
		})
	})
});
