var counter = 0;

function timeloop() {
	counter = counter + 1;
	getSettings();
	if (counter > 60 && navSelect != "save") { // refresh index list but not when navigation is in save tab
		load_memIndexList();
		counter = 0;
	}
	setTimeout("timeloop()",1000);
}

function afterLogin() {
	adminCheck();
	var loadingTimeout = setTimeout(function() {
		$.mobile.loading( "show", {
			text: "verbinden",
			textVisible: true
		});
	}, 500);
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		url: server.protocol+"://"+server.host+":"+server.port+"/all",
		timeout: 5000
	})
	.done(function(data) {
		setCookie("auth", auth.string); // next time no login
		setCookie("username", auth.un);
		clearTimeout(loadingTimeout);
		$("#wrapper").show();
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
		option = data.option;
		if (!option.loaded)	{
			option.loaded = {};
			option.loaded.slot = 1;
			$("#loaded").hide();
		}
		set_area();
		set_option();
		set_lineLengh();
		draw_border();
		draw(line.set);
		draw(line.current);
		draw(line.target);
		animation();
		load_memIndexList();
		$.mobile.loading( "hide" )
		setTimeout("timeloop()",1000);
	})
	.fail(function(a,b,e) {
		clearTimeout(loadingTimeout);
		var message
		if (!e.responseText) {
			message = b
		} else {
			message = e.responseText
		}
		$.mobile.loading( "show", {
			text: message,
			textVisible: true,
			textonly: true
		});
		setTimeout("location.reload()",30000);
	})
};

function getSettings() {
	adminCheck();
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		url: server.protocol+"://"+server.host+":"+server.port+"/all",
		timeout: 1000
	})
	.done(function(data) {
		errorCount = 0;
		$.mobile.loading( "hide" );
		if (auth.un != "admin") {
			area = Object.assign(area, data.area);
			set_area();
			draw_border();
			option = data.option;
			set_option();
		}
		var tempFront = line.current.lengh.front
		var tempBack  = line.current.lengh.back
		line.current 	 = Object.assign(line.current, 	 data.line.current);
		line.target  	 = Object.assign(line.target,    data.line.target );
		//line.speed   	 = Object.assign(line.speed,  	 data.line.speed  );
		line.animation = Object.assign(line.animation, data.line.current);
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
	.fail(function(e) {
		errorCount = errorCount + 1;
		if (errorCount>3) {
			var message
			if (e.responseText == "") {message = "keine Verbindung"} else {message = e.responseText}
			$.mobile.loading( "show", {
				text: message,
				textVisible: true,
				textonly: true
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
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		url: server.protocol+"://"+server.host+":"+server.port+"/area",
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

function get_option() {
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		url: server.protocol+"://"+server.host+":"+server.port+"/option",
		timeout: 2000
	})
	.done(function(response) {
		$.mobile.loading( "hide" );
		option = response;
		set_option();
	})
	.fail(function() {
		console.log("error getting option");
		setTimeout("get_option()",4000);
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
	if (auth.un != "admin") {
		if (x > area.range.x.max) x = area.range.x.max;
		if (x < area.range.x.min) x = area.range.x.min;
		if (y > area.range.y.max) y = area.range.y.max;
		if (y < area.range.y.min) y = area.range.y.min;
	}
	if (x) {
		line.set.coordinate = {};
		line.set.coordinate.x = x;
		line.set.coordinate.y = y;
	}
}

function adminCheck() {
	if (auth.un == "admin") {
		$("#admin").show();
	} else {
		$("#admin").hide();
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
			if (memory[index].name == "") memory[index].name = "- ohne Bezeichnung -"
			if (memory[index].x == 0) { memory[index].x = 5500; memory[index].y = 2200; }
			//if (index > 0 && index <= 9)
			$("#memIndexList").append('<li class="indexListTop"><a href=# onclick="callSlot('+index+');">'+memory[index].name+'</a><div class="ui-grid-a"><div class="ui-block-a"><div class="ui-bar">Höhe:<br>'+parseInt((memory[index].y/10)+0.5)+' cm</div></div><div class="ui-block-b"><div class="ui-bar">Tiefe:<br>'+parseInt((memory[index].x/10)+0.5)+' cm</div></div></div></li>');
		}
	});
	if (option.loaded) $("#memIndexSlider").val(option.loaded.slot).change();
	$('#memIndexList').listview('refresh');
	if (navSelect == "load") $(".indexListTop").css({"border-top": "0.2em solid #cce6b3"});
	if (navSelect == "save") $(".indexListTop").css({"border-top": "0.2em solid #ffccb3"});
}

function callSlot(e) {
	if (navSelect == "load") {
		if ($.isEmptyObject(memory[e])) {
			$.mobile.loading( "show", {
				text: "leer",
				textVisible: true,
				textonly: true
			})
		} else {
			loadSlot(e);
		}
	}
	if (navSelect == "save") {
		if (auth.un != "admin" && (e == 1 || e == 100)) {
			$.mobile.loading( "show", {
				text: "schreibgeschützt",
				textVisible: true,
				textonly: true
			});
		} else {
			if (!$.isEmptyObject(memory[e])) {
				$("#popupConfirm").attr("onclick","saveSlot("+e+");")
				$("#popupDialog").popup("open");
			} else {
				saveSlot(e);
			}
		}
	}
};

function loadSlot(e) {
	var loadPackage = {};
 	var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "show", {
                        text: "laden",
                        textVisible: true
                });
        }, 500);
	loadPackage.slot = e
	loadPackage.x 	 = memory[e].x;
	loadPackage.y 	 = memory[e].y;
	loadPackage.name = memory[e].name;
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/load", // only writing slot
		timeout: 5000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(loadPackage)
	})
	.done(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "hide" );
		line.set.coordinate.x = memory[e].x;
		line.set.coordinate.y = memory[e].y;
		draw(line.set);
		option.loaded = {};
		option.loaded.slot = loadPackage.slot;
		option.loaded.name = loadPackage.name;
		$("#loaded").html(loadPackage.name);
		$("#loaded").show();

	})
	.fail(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
}

function saveSlot(e) {
	var savePackage = {};
	var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "show", {
                        text: "speichern",
                        textVisible: true
                });
        }, 500);
	var x, y
	x = (Math.pow(line.current.lengh.front,2)+Math.pow(area.dimension.x,2)-Math.pow(line.current.lengh.back,2))/(2*area.dimension.x);
	y = Math.sqrt(Math.pow(line.current.lengh.front,2)-Math.pow(x,2)-Math.pow(area.dimension.z/2,2));
	line.current.coordinate.x = x;
	line.current.coordinate.y = area.dimension.y - y;
	savePackage[e] = {}
	savePackage[e].x = parseInt(line.current.coordinate.x);
	savePackage[e].y = parseInt(line.current.coordinate.y);
	if (savePackage[e].name != "- leer -" && savePackage[e].name != "- ohne Bezeichnung -") {
		savePackage[e].name = $("#memIndexName").val();
	}
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		method: "POST",
		url: server.protocol+"://"+server.host+":"+server.port+"/writeMemory",
		timeout: 5000,
		dataType: "text",
		contentType: "application/json; charset=utf-8",
		data: JSON.stringify(savePackage)
	})
	.done(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "hide" );
		memory[e] = {}
		memory[e].x = parseInt(line.current.coordinate.x);
		memory[e].y = parseInt(line.current.coordinate.y);
		memory[e].name = $("#memIndexName").val();
		loadSlot(e);
		setTimeout("load_memIndexList()", 1000);
	})
	.fail(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: "fehlgeschlagen",
			textVisible: true,
			textonly: true
		})
	})
}

function load_memIndexList(){
	 var loadingTimeout = setTimeout(function() {
                $.mobile.loading( "show", {
                        text: "laden",
                        textVisible: true
                });
        }, 1000);
	$.ajax({
		xhrFields: { withCredentials: true },
		beforeSend: function (xhr) { xhr.setRequestHeader('Authorization', 'Basic ' + auth.string) },
		url: server.protocol+"://"+server.host+":"+server.port+"/readMemory",
		timeout: 2000
	})
	.done(function(response) {
		clearTimeout(loadingTimeout);
                $.mobile.loading( "hide" );
		memory = response;
		create_memIndexList();
		create_deleteIndexList();
	})
	.fail(function() {
		clearTimeout(loadingTimeout);
		$.mobile.loading( "show", {
			text: "Function failed: load_memIndexList()",
			textVisible: true,
			textonly: true
		})
	})
}

function empty() {
	$.mobile.loading( "show", {
		text: "leer",
		textVisible: true,
		textonly: true
	})
}

function setCookie(cname, cvalue) {
    var d = new Date();
	cvalue = cvalue.replace(/;/g,"\\semi");
    d.setTime(d.getTime() + (30*24*60*60*1000));
    var expires = "expires="+ d.toUTCString();
    document.cookie = cname + "=" + cvalue + "; " + expires;
}

function getCookie(cname) {
    var name = cname + "=";
	var cvalue;
    var ca = document.cookie.split(';');
    for(var i = 0; i <ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0)==' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
			cvalue = c.substring(name.length,c.length);
			cvalue = cvalue.replace(/\\semi/g,";");
            return cvalue;
        }
    }
    return "";
}

function delCookie(cname) {
    document.cookie = cname+'=; Max-Age=-99999999;';
}
