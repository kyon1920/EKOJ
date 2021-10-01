// 加载题目 ------------------------------------------------------------------
//根据得到的行对象得到所在的行数
function getRowObj(obj) {
	var i = 0;
	while(obj.tagName.toLowerCase() != "tr") {
		obj = obj.parentNode;
		if(obj.tagName.toLowerCase() == "table")
		return null;
	}
	return obj;
}

function getRowNo(obj) {
	var trObj = getRowObj(obj);
	var trArr = trObj.parentNode.children;
	for(var trNo= 0; trNo < trArr.length; trNo++) {
		if(trObj == trObj.parentNode.children[trNo]) {
			load4namepro(trNo + 1);
		}
	}
}

function load4namepro() {
	alert("xxx");
	var number=document.getElementById("number").value;
	alert(number);
	var tabs=document.getElementById("testxxx").value;
	alert(tabs);
}
// --------------------------------------------------------------------------

