
function timeloop() {
	if (document.location.hash == "#admin") {
		getLines();
	} else {
		getSettings();
	}
	setTimeout("timeloop()",1000);
}

function getSettings() {
	adminCheck();
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/all",
	//cache: false,
	timeout: 1000
	})
	.done(function(data) {
		errorCount = 0;
		$.mobile.loading( "hide" );
		area = Object.assign(area, data.area);
		set_area();
		draw_border();
		option = Object.assign(option, data.option);
		set_option();
		var tempFront = line.current.lengh.front
		var tempBack  = line.current.lengh.back
		line.current 	 = Object.assign(line.current, 	 data.line.current);
		line.target  	 = Object.assign(line.target,    data.line.target );
		draw(line.target);
		set_lineLengh();
		//line.speed   	 = Object.assign(line.speed,  	 data.line.speed  );
		line.animation = Object.assign(line.animation, data.line.current);
		if (line.speed.lengh.front == 0 || line.speed.lengh.back == 0) {
			line.speed.lengh.front = (line.current.lengh.front - tempFront) /2;
			line.speed.lengh.back  = (line.current.lengh.back  - tempBack ) /2;
		} else {
			line.speed.lengh.front = line.speed.lengh.front + (line.current.lengh.front - tempFront) /2;
			line.speed.lengh.back  = line.speed.lengh.back  + (line.current.lengh.back  - tempBack ) /2;
		}
	})
	.fail(function() {
		errorCount = errorCount + 1;
		if (errorCount>3) {
			$.mobile.loading( "show", {
				text: "keine Verbindung",
				textVisible: true
			});
		}
		if (errorCount>10) {
			location.reload();
		}
	})
};

function getLines() {
	adminCheck();
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/loop",
	//cache: false,
	timeout: 2000
	})
	.done(function(response) {
		errorCount = 0;
		$.mobile.loading( "hide" );
		var tempFront = line.current.lengh.front
		var tempBack  = line.current.lengh.back
		line.current 	 = Object.assign(line.current, 	response.current);
		line.target  	 = Object.assign(line.target,  	response.target);
		//line.speed   	 = Object.assign(line.speed,  	response.speed);
		line.animation = Object.assign(line.animation, response.current);
		if (line.speed.lengh.front == 0 || line.speed.lengh.back == 0) {
			line.speed.lengh.front = (line.current.lengh.front - tempFront) /2;
			line.speed.lengh.back  = (line.current.lengh.back  - tempBack ) /2;
		} else {
			line.speed.lengh.front = line.speed.lengh.front + (line.current.lengh.front - tempFront) /2;
			line.speed.lengh.back  = line.speed.lengh.back  + (line.current.lengh.back  - tempBack ) /2;
		}
		draw(line.target);
		set_lineLengh();
	})
	.fail(function() {
		errorCount = errorCount + 1;
		if (errorCount>3) {
			$.mobile.loading( "show", {
				text: "keine Verbindung",
				textVisible: true
			});
		}
		if (errorCount>10) {
			location.reload();
		}
	})
};

function animation () {
	if (line.speed.lengh.front > 0 && line.current.lengh.front > line.target.lengh.front) line.speed.lengh.front = 0;
	if (line.speed.lengh.front < 0 && line.current.lengh.front < line.target.lengh.front) line.speed.lengh.front = 0;
	if (line.speed.lengh.back  > 0 && line.current.lengh.back  > line.target.lengh.back ) line.speed.lengh.back  = 0;
	if (line.speed.lengh.back  < 0 && line.current.lengh.back  < line.target.lengh.back ) line.speed.lengh.back  = 0;

	if (line.current.lengh.front != line.target.lengh.front || line.current.lengh.back != line.target.lengh.back) {
		line.animation.lengh.front += (line.speed.lengh.front / 10);
		line.animation.lengh.back  += (line.speed.lengh.back  / 10);
	} else {
		line.animation.lengh.front = line.current.lengh.front;
		line.animation.lengh.back  = line.current.lengh.back;
		line.speed.lengh.front = 0;
		line.speed.lengh.back = 0;
	}
	draw(line.animation);
	setTimeout("animation()",100);
}


function getArea() {
	$.mobile.loading( "show" );
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/area",
	//cache: false,
	timeout: 2000
	})
	.done(function(response) {
		$.mobile.loading( "hide" );
		area = Object.assign(area, response);
		set_area();
		draw_border();
	})
	.fail(function() {
		console.log("error getting area");
		setTimeout("getArea()",4000);
	})
};

function getOption() {
	$.mobile.loading( "show" );
	$.ajax({
	url: server.protocol+"://"+server.host+":"+server.port+"/option",
	//cache: false,
	timeout: 2000
	})
	.done(function(response) {
		$.mobile.loading( "hide" );
		option = Object.assign(option, response);
		set_option();
	})
	.fail(function() {
		console.log("error getting option");
		setTimeout("getOption()",4000);
	})
};

function draw(obj) {
	var x, y
	if (obj.id == "current" || obj.id == "target") {
		x = (Math.pow(obj.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(obj.lengh.back,2))/(2*area.dimension.x)
		y = Math.sqrt(Math.pow(obj.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
		obj.coordinate = {};
		obj.coordinate.x = x
		obj.coordinate.y = area.dimension.y - y
	}
	x = (area.dimension.x - obj.coordinate.x) * 600 / area.dimension.x;
	y = (area.dimension.y - obj.coordinate.y) * 400 / area.dimension.y;
	$("#"+obj.id+"-line1").attr("points", x+","+y+" 600,0");
	$("#"+obj.id+"-line2").attr("points", x+","+y+"   0,0");
	$("#"+obj.id+"-mic").attr("cx", x);
	$("#"+obj.id+"-mic").attr("cy", y);
	$("#x_num_"+obj.id).html(parseInt((obj.coordinate.x/10)+0.5));
	$("#y_num_"+obj.id).html(parseInt((obj.coordinate.y/10)+0.5));
	if (obj.id == "set") {
		$("#directCoordinateY").attr('value',parseInt((obj.coordinate.y/10)+0.5));
		$("#directCoordinateX").attr('value',parseInt((obj.coordinate.x/10)+0.5));
	}
}

function vwTOpx(value) {
    var w = window,
      d = document,
      e = d.documentElement,
      g = d.getElementsByTagName("body")[0],
      x = w.innerWidth || e.clientWidth || g.clientWidth;
    return (x*value)/100;
}

function emTOpx(value) {
	return(16*value);
}

function check_set (x,y) {
	if (x > area.range.x.max) x = area.range.x.max;
	if (x < area.range.x.min) x = area.range.x.min;
	if (y > area.range.y.max) y = area.range.y.max;
	if (y < area.range.y.min) y = area.range.y.min;
	if (x) {
		line.set.coordinate = {};
		line.set.coordinate.x = x;
		line.set.coordinate.y = y;
	}
}

function adminCheck() {
	if (document.location.hash == "#admin") {
		// include harware switch on master device
		$("#admin").show();
	}
}

function draw_border() {
	var x, y, width, height
	x = 600 - (area.range.x.max * 600 / area.dimension.x);
	y = 400 - (area.range.y.max * 400 / area.dimension.y);
	width  = (area.range.x.max - area.range.x.min) * 600 / area.dimension.x;
	height = (area.range.y.max - area.range.y.min) * 400 / area.dimension.y;
	$("#range").attr("x",x).attr("y",y).attr("width",width).attr("height",height)
}

function create_memIndexList() {
	$("#memIndexList").html("");
	Object.keys(memory).forEach(function (index, element) {
		if ( index > 0 && index <= 100 ) {
			if (index == 1) memory[index].name = "Standart";
			if (index == 2) memory[index].name = "Gemeindechor";
			if (index == 3) memory[index].name = "Jugendchor";
			if (index == 4) memory[index].name = "Jungscharchor";
			if (index == 5) memory[index].name = "Kinderchor 1";
			if (index == 6) memory[index].name = "Kinderchor 2";
			if (index == 7) memory[index].name = "Blasorchester";
			if (index == 8) memory[index].name = "Streichorchester";
			if (index == 9) memory[index].name = "Sinfonieorchester";
			if (index == 100) memory[index].name = "Parkposition";
			if (memory[index].x == 0) { memory[index].x = 5500; memory[index].y = 2200; }
			if (index > 0 && index <= 9) $("#memIndexList").append('<li class="indexListTop"><a href=# onclick="callSlot('+index+');">'+memory[index].name+'</a><div class="ui-grid-a"><div class="ui-block-a"><div class="ui-bar">Höhe:<br>'+parseInt((memory[index].y/10)+0.5)+' cm</div></div><div class="ui-block-b"><div class="ui-bar">Tiefe:<br>'+parseInt((memory[index].x/10)+0.5)+' cm</div></div></div></li>');
		}
	});
	$("#memIndexSlider").val(memory.slot).change();
	$('#memIndexList').listview('refresh');
	if (navSelect == "load") $(".indexListTop").css({"border-top": "0.2em solid #ccffcc"});
	if (navSelect == "save") $(".indexListTop").css({"border-top": "0.2em solid #ffcccc"});
}

function callSlot(e) {
	memory.slot = e;
	if (navSelect == "load") {
		loadSlot(memory.slot)
	}
	if (navSelect == "save") {
		if (document.location.hash != "#schreibschutz" && (memory.slot == 1 || memory.slot == 100)) {
			$.mobile.loading( "show", {
				text: "schreibgeschützt",
				textVisible: true,
				textonly: true
			});
		} else {
			if (memory[memory.slot].x) {
				$("#popupConfirm").attr("onclick","saveSlot("+memory.slot+");")
				$("#popupDialog").popup("open");
			} else {
				saveSlot(memory.slot);
			}
		}
	}
};

function loadSlot(e) {
	var loadPackage = {};
	memory.slot = e;
	$.mobile.loading( "show", {
		text: "laden",
		textVisible: true
	});
	loadPackage.slot = memory.slot
	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/writeMemory", // only writing slot
	timeout: 5000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(loadPackage)
	})
	.done(function() {
		line.set.coordinate.x = memory[memory.slot].x;
		line.set.coordinate.y = memory[memory.slot].y;
		draw(line.set);
		$("#send").trigger("click");

	})
	.fail(function() {
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
}

function saveSlot(e) {
	var savePackage = {};
	memory.slot = e;
	$.mobile.loading( "show", {
		text: "speichern",
		textVisible: true
	});
	var x, y
	x = (Math.pow(line.current.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(line.current.lengh.back,2))/(2*area.dimension.x);
	y = Math.sqrt(Math.pow(line.current.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
	line.current.coordinate.x = x;
	line.current.coordinate.y = area.dimension.y - y;
	savePackage[memory.slot] = {}
	savePackage[memory.slot].x = parseInt(line.current.coordinate.x);
	savePackage[memory.slot].y = parseInt(line.current.coordinate.y);
	savePackage.slot = memory.slot

	$.ajax({
	method: "POST",
	url: server.protocol+"://"+server.host+":"+server.port+"/writeMemory",
	timeout: 5000,
	dataType: "text",
	contentType: "application/json; charset=utf-8",
	data: JSON.stringify(savePackage)
	})
	.done(function() {
		$.mobile.loading( "hide" );
		memory[memory.slot].x = parseInt(line.current.coordinate.x);
		memory[memory.slot].y = parseInt(line.current.coordinate.y);
		memory[memory.slot].name = "";
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
