// 信息 ---------------------------------------------------------------------
var islogin = false;
var username;    // 用户名
var passwd;      // 密码
var usertype;    // 用户类型
var name;        // 名字
var material;    // 用户信息
var classid;     // 班级ID

var number4exercise;   // 练习题总数
var page4exercise = 0; // 当前页数
var nowexerpro;        // 当前题id

var tpid;              // 试卷id
var tpname;            // 试卷名字
var tusername;
var tcodeid;           // 试卷编程题id
var tcodenumber;       // 试卷编程题数量
var starttime;
var timelimit;
var selectpronumber;
var judgepronumber;
var codepronumber;
var nowloadclassid;

var nowloadtestpaperselectnumber;
var nowloadtestpaperjudgenumber;
var nowloadtestpapercodenumber;

var isend = false;

var file_in_1_content = "";
var file_in_2_content = "";
var file_in_3_content = "";

var file_out_1_content = "";
var file_out_2_content = "";
var file_out_3_content = "";

function sleep(numberMillis) {
    var now = new Date();
    var exitTime = now.getTime() + numberMillis;
    while (true) {
        now = new Date();
        if (now.getTime() > exitTime)
        return;
    }
}

// ajax发送请求 --------------------------------------------------------------
function sendRequest(reqinfo) {
    var xmlhttp;
    if (window.XMLHttpRequest) {
        xmlhttp = new XMLHttpRequest();  //  IE7+, Firefox, Chrome, Opera, Safari 浏览器执行代码
    }
    else {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");  // IE6, IE5 浏览器执行代码
    }
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
            dealinfo(xmlhttp.responseText);
        }
    }
    xmlhttp.open("POST", "", false);
    xmlhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xmlhttp.send(reqinfo);
}
// --------------------------------------------------------------------------

// 消息处理 ------------------------------------------------------------------
function dealinfo(resinfo) {
    var data = eval('(' + resinfo + ')');
    
    if(data.infotype == "LOGIN") {
        if(data.result == "SU") {
            username = loginusername;
            passwd = loginpasswd;
            usertype = data.usertype;
            name = data.name;
            material = data.material;
            classid = data.classid;
            islogin = true;
            localStorage.setItem('islogin', islogin);
            localStorage.setItem('username', username);
            localStorage.setItem('password', passwd);
            localStorage.setItem('usertype', usertype);
            localStorage.setItem('name', name);
            localStorage.setItem('material', material);
            localStorage.setItem('classid', classid);
			if(data.usertype == "0") {
				window.location.href = "./student/index.html";
			}
			else if(data.usertype == "1") {
				window.location.href = "./teacher/index.html";
			}
			else if(data.usertype == "2") {
				window.location.href = "./admin/index.html";
			}
        }
        else {
            alert("🎃登录失败，检查用户名和密码！🎃");
        }
    }
    else if(data.infotype == "LOGUP") {
        if(data.result == "SU") {
            alert("🎉注册成功，快去登录吧~🎉");
        }
        else {
            alert("😭注册失败，更改用户名后重试！😭");
        }
    }
    else if(data.infotype == "EXERCISELIST") {
        var aObj = document.getElementById("pages4exercise");
        number4exercise = data.number4exercise;
        localStorage.setItem('number4exercise', data.number4exercise);
        aObj.innerText = Math.ceil(number4exercise/20);

        // 清空表格数据
        let artTable = document.getElementById("exercisecustomers");
        var len = artTable.rows.length;  
        for(var i = 0; i < len; i++) {
            artTable.deleteRow(0);
        }

        let htr = document.createElement("tr");
        let hth1 = document.createElement("th");
        hth1.innerHTML = "&nbsp;&nbsp;";
        htr.appendChild(hth1);

        let hth2 = document.createElement("th");
        hth2.innerHTML = "&nbsp;&nbsp;#";
        htr.appendChild(hth2);

        let hth3 = document.createElement("th");
        hth3.innerHTML = "&nbsp;&nbsp;题目描述";
        hth3.setAttribute("width", "50%");
        htr.appendChild(hth3);

        let hth4 = document.createElement("th");
        hth4.innerHTML = "&nbsp;&nbsp;难度";
        htr.appendChild(hth4);

        let hth5 = document.createElement("th");
        hth5.innerHTML ="&nbsp;&nbsp;通过率";
        htr.appendChild(hth5);

        let hth6 = document.createElement("th");
        hth6.innerHTML = "&nbsp;&nbsp;标签";
        hth6.setAttribute("width", "25%");
        htr.appendChild(hth6);
        artTable.appendChild(htr);

        for(let i = 0; i < data.exerciselist.length; ++i) {
            var tr = document.createElement("tr");
            if((i%2) != 0) {
                tr.setAttribute("class", "alt");
            }
            var td1 = document.createElement("td");;
            td1.innerHTML = " ";
            tr.appendChild(td1);
            var td2 = document.createElement("td");
            td2.innerHTML = data.exerciselist[i].id;
            tr.appendChild(td2);
            var td3 = document.createElement("td");
            td3.innerHTML = "<a href=\"exerpro.html\" onclick=\"loadexerpro(" + (i + 1) + ")\" target=\"_Self\">" + data.exerciselist[i].name + "</a>";
            tr.appendChild(td3);
            var td4 = document.createElement("td");
            if(data.exerciselist[i].level == 0) {
                td4.innerHTML = "简单";
            }
            if(data.exerciselist[i].level == 1) {
                td4.innerHTML = "中等";
            }
            if(data.exerciselist[i].level == 2) {
                td4.innerHTML = "困难";
            }
            tr.appendChild(td4);
            var td5 = document.createElement("td");
            if(data.exerciselist[i].submission == 0) {
                td5.innerHTML = "0.00%";
            }
            else {
                let str = (data.exerciselist[i].pass / data.exerciselist[i].submission * 100).toFixed(2);
                td5.innerHTML = str + "%";
            }
            
            tr.appendChild(td5);
            var td6 = document.createElement("td");
            td6.innerHTML = data.exerciselist[i].attribute;
            tr.appendChild(td6);
            artTable.appendChild(tr);
        }
    }
    else if(data.infotype == "LOADEXERPRO") {
        let name4pro = document.getElementById("name4exerpro");
        name4pro.innerHTML = nowexerpro + ". " + data.name;

        let level4exerpro = document.getElementById("level4exerpro");
        if(data.level == 0) {
            level4exerpro.innerHTML = "简单";
        }
        else if(data.level == 1) {
            level4exerpro.innerHTML = "中等";
        }
        else if(data.level == 2) {
            level4exerpro.innerHTML = "困难";
        }

        let exerprodiscription = document.getElementById("exerprodiscription");
        exerprodiscription.innerHTML = data.description;

        let exerproexplain = document.getElementById("exerproexplain");
        exerproexplain.innerHTML = data.explain;

        let exerproexample1 = document.getElementById("exerproexample1");
        exerproexample1.innerHTML = data.examples1;

        let exerproexample2 = document.getElementById("exerproexample2");
        exerproexample2.innerHTML = data.examples2;
    }
    else if(data.infotype == "RUNCODE") {
        alert(data.status);
        alert(data.ratio);
        let result = document.getElementById("runresult4exercise");
        if(data.status == 7) {
            result.innerHTML = "编译未通过：" + data.resultstr;
        }
        else if(data.status == 1) {
            result.innerHTML = "结果错误：" + data.resultstr;
        }
        else if(data.status == 2) {
            result.innerHTML = "运行超时：" + data.resultstr;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 0) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 8) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 1) {
            result.innerHTML = "完全正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory;
        }
    }
    else if(data.infotype == "SUBMITEXAM") {
        if(data.result == "SU") {
            alert("创建成功~");
        }
        else {
            alert("创建失败，请重试！");
        }
    }
    else if(data.infotype == "LOADTESTPAPER") {
        var testpapertable = document.getElementById("testpaper4list");
        for(let i = 0; i < data.testpaperlist.length; ++i) {
            var tr = document.createElement("tr");
            var td1 = document.createElement("td");;
            td1.innerHTML = data.testpaperlist[i].name;
            tr.appendChild(td1);
            var td2 = document.createElement("td");
            let str = "<a href=\"testpaper.html\" onclick=\"load4TestPaperPro('" + data.testpaperlist[i].tpid + "', '" + data.testpaperlist[i].name + "')\" target=\"details\">[详情]</a>";
            td2.innerHTML = str;
            tr.appendChild(td2);
            testpapertable.appendChild(tr);
        }
    }
    else if(data.infotype == "LOADTEACHERTESTPAPERPRO") {
        tpname = localStorage.getItem('tpname');
        username = localStorage.getItem('username');
        let ttestpapername = document.getElementById("teachertestpapername");
        ttestpapername.innerHTML = tpname;
        let tusername = document.getElementById("teacherusername");
        tusername.innerHTML = username;
        localStorage.setItem('nowloadtestpaperselectnumber', data.select.length);
        localStorage.setItem('nowloadtestpaperjudgenumber', data.judge.length);
        localStorage.setItem('nowloadtestpapercodenumber', data.codelist.length);

        let select4classid = document.getElementById("select4classid");
        for(let i = 0; i < data.class.length; ++i) {
            var option = document.createElement("option");
            option.innerHTML = data.class[i].id;
            select4classid.appendChild(option);
        }

        var selectpro = document.getElementById("paperselectpros");
        if(data.select.length > 0) {
            let headselectpro = document.createElement("h3");
            headselectpro.innerHTML = "选择题";
            selectpro.appendChild(headselectpro);
        }
        for(let i = 0; i < data.select.length; ++i) {
            var selectspan = document.createElement("span");
            let number = i + 1;
            selectspan.innerHTML = number.toString() + ". " + data.select[i].discription;
            selectpro.appendChild(selectspan);

            let brA = document.createElement("br");
            selectpro.appendChild(brA);
            var selectspannbspA = document.createElement("span");
            selectspannbspA.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspA);
            var selectA = document.createElement("input");
            selectA.setAttribute("type", "radio");
            let name_A = number + "_select";
            selectA.setAttribute("name", name_A);
            selectA.setAttribute("value", "A");
            selectpro.appendChild(selectA);
            var selectspanA = document.createElement("span");
            selectspanA.innerHTML = "&nbsp;&nbsp;A: " + data.select[i].A;
            selectpro.appendChild(selectspanA);

            let brB = document.createElement("br");
            selectpro.appendChild(brB);
            var selectspannbspB = document.createElement("span");
            selectspannbspB.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspB);
            var selectB = document.createElement("input");
            selectB.setAttribute("type", "radio");
            let name_B = number + "_select";
            selectB.setAttribute("name", name_B);
            selectB.setAttribute("value", "B");
            selectpro.appendChild(selectB);
            var selectspanB = document.createElement("span");
            selectspanB.innerHTML = "&nbsp;&nbsp;B: " + data.select[i].B;
            selectpro.appendChild(selectspanB);

            let brC = document.createElement("br");
            selectpro.appendChild(brC);
            var selectspannbspC = document.createElement("span");
            selectspannbspC.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspC);
            var selectC = document.createElement("input");
            selectC.setAttribute("type", "radio");
            let name_C = number + "_select";
            selectC.setAttribute("name", name_C);
            selectC.setAttribute("value", "C");
            selectpro.appendChild(selectC);
            var selectspanC = document.createElement("span");
            selectspanC.innerHTML = "&nbsp;&nbsp;C: " + data.select[i].C;
            selectpro.appendChild(selectspanC);

            let brD = document.createElement("br");
            selectpro.appendChild(brD);
            var selectspannbspD = document.createElement("span");
            selectspannbspD.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspD);
            var selectD = document.createElement("input");
            selectD.setAttribute("type", "radio");
            let name_D = number + "_select";
            selectD.setAttribute("name", name_D);
            selectD.setAttribute("value", "D");
            selectpro.appendChild(selectD);
            var selectspanD = document.createElement("span");
            selectspanD.innerHTML = "&nbsp;&nbsp;D: " + data.select[i].D;
            selectpro.appendChild(selectspanD);

            if(data.select[i].answer == "A") {
                selectA.setAttribute("checked", "true");
            }
            else if(data.select[i].answer == "B") {
                selectB.setAttribute("checked", "true");
            }
            else if(data.select[i].answer == "C") {
                selectC.setAttribute("checked", "true");
            }
            else if(data.select[i].answer == "D") {
                selectD.setAttribute("checked", "true");
            }

            let brE = document.createElement("br");
            selectpro.appendChild(brE);
        }

        var judgepro = document.getElementById("paperjudgepros");
        if(data.judge.length > 0) {
            let headjudgepro = document.createElement("h3");
            headjudgepro.innerHTML = "判断题";
            judgepro.appendChild(headjudgepro);
        }
        for(let i = 0; i < data.judge.length; ++i) {
            var judgespan = document.createElement("span");
            let number = i + 1;
            judgespan.innerHTML = number.toString() + ". " + data.judge[i].discription;
            judgepro.appendChild(judgespan);

            let brA = document.createElement("br");
            judgepro.appendChild(brA);
            var judgespannbspA = document.createElement("span");
            judgespannbspA.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            judgepro.appendChild(judgespannbspA);
            var judgeA = document.createElement("input");
            judgeA.setAttribute("type", "radio");
            let name_A = number + "_judge";
            judgeA.setAttribute("name", name_A);
            judgeA.setAttribute("value", "A");
            judgepro.appendChild(judgeA);
            var judgespanA = document.createElement("span");
            judgespanA.innerHTML = "&nbsp;&nbsp;A: 正确";
            judgepro.appendChild(judgespanA);

            let brB = document.createElement("br");
            judgepro.appendChild(brB);
            var judgespannbspB = document.createElement("span");
            judgespannbspB.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            judgepro.appendChild(judgespannbspB);
            var judgeB = document.createElement("input");
            judgeB.setAttribute("type", "radio");
            let name_B = number + "_judge";
            judgeB.setAttribute("name", name_B);
            judgeB.setAttribute("value", "B");
            judgepro.appendChild(judgeB);
            var judgespanB = document.createElement("span");
            judgespanB.innerHTML = "&nbsp;&nbsp;B: 错误";
            judgepro.appendChild(judgespanB);

            if(data.judge[i].answer == "A") {
                judgeA.setAttribute("checked", "true");
            }
            else if(data.judge[i].answer == "B") {
                judgeB.setAttribute("checked", "true");
            }
            let brcC = document.createElement("br");
            judgepro.appendChild(brcC);
        }

        var codepro = document.getElementById("papercodepros");
        tcodenumber = data.codelist.length;
        localStorage.setItem('tcodenumber', tcodenumber);
        if(tcodenumber > 0) {
            let headcodepro = document.createElement("h3");
            headcodepro.innerHTML = "编程题";
            codepro.appendChild(headcodepro);
        }
        for(let i = 0; i < tcodenumber; ++i) {
            let br = document.createElement("br");
            codepro.appendChild(br);
            let title_a = document.createElement("a");
            title_a.setAttribute("href", "exerpro.html");
            title_a.setAttribute("target", "details");

            let fun = "load4teachertestpapercodepro(" + i + ")";
            title_a.setAttribute("onclick", fun);
            let number = i + 1;
            title_a.innerHTML = number + ". [" + data.codelist[i].name + "]";
            codepro.appendChild(title_a);
            let brr = document.createElement("br");
            codepro.appendChild(brr);
        }
    }
    else if(data.infotype == "LOADTEACHERTESTPAPERCODEPRO") {
        tcodeid = localStorage.getItem('tcodeid');
        let name4pro = document.getElementById("name4exerpro");
        let id = parseInt(tcodeid) + 1;
        name4pro.innerHTML =  id + ". " + data.name;

        let exerprodiscription = document.getElementById("exerprodiscription");
        exerprodiscription.innerHTML = data.description;

        let exerproexplain = document.getElementById("exerproexplain");
        exerproexplain.innerHTML = data.explain;

        let exerproexample1 = document.getElementById("exerproexample1");
        exerproexample1.innerHTML = data.examples1;

        let exerproexample2 = document.getElementById("exerproexample2");
        exerproexample2.innerHTML = data.examples2;
    }
    else if(data.infotype == "SUBMITTESTPAPER") {
        if(data.result == "SU") {
            alert("发布成功，请刷新~");
        }
        else {
            alert("发布失败，请重试！");
        }
    }
    else if(data.infotype == "DELETETESTPAPER") {
        if(data.result == "SU") {
            alert("删除成功，请刷新~");
        }
        else {
            alert("删除失败，请重试！");
        }
    }
    else if(data.infotype == "LOADSTUCLASS") {
        if(data.result == "SU") {
            classid = data.classid;
            localStorage.setItem('classid', classid);
            let head = document.getElementById("examinfo");
            head.innerHTML = "考试信息如下：";
            let tablediv = document.getElementById("examlist");
            let examtable = document.createElement("table");
            examtable.setAttribute("style", "margin:auto");
            tablediv.appendChild(examtable);
            let headth = document.createElement("tr");
            let headtd1 = document.createElement("th");
            headtd1.innerHTML = "试卷名称";
            headtd1.setAttribute("width", "40%");
            headth.appendChild(headtd1);
            let headtd2 = document.createElement("th");
            headtd2.innerHTML = "老师";
            headtd2.setAttribute("width", "10%");
            headth.appendChild(headtd2);
            let headtd3 = document.createElement("th");
            headtd3.innerHTML = "发布时间";
            headtd3.setAttribute("width", "30%");
            headth.appendChild(headtd3);
            let headtd4 = document.createElement("th");
            headtd4.innerHTML = "限时（分钟）";
            headtd4.setAttribute("width", "20%");
            headth.appendChild(headtd4);
            examtable.appendChild(headth);

            for(let i = 0; i < data.testpaperlist.length; ++i) {
                let time = data.testpaperlist[i].starttime * 1000;
                let timelimit = data.testpaperlist[i].timelimit;
                let tr = document.createElement("tr");
                let td1 = document.createElement("td");
                let str = "<a href=\"exam.html\" target=\"htmlform\" onclick=\"load4stutestpaper('"
                            + data.testpaperlist[i].tpid + "', '" + data.testpaperlist[i].tpname + "', '" + time + "', '" + timelimit + "', '" + data.testpaperlist[i].tusername + "')\">"
                            + data.testpaperlist[i].tpname + "</a>"
                td1.innerHTML = str;
                tr.appendChild(td1);

                let td2 = document.createElement("td");
                td2.innerHTML = data.testpaperlist[i].tusername;
                tr.appendChild(td2);

                let td3 = document.createElement("td");
                let date = new Date(time);
                var YY = date.getFullYear() + '-';
                var MM = (date.getMonth() + 1 < 10 ? '0' + (date.getMonth() + 1) : date.getMonth() + 1) + '-';
                var DD = (date.getDate() < 10 ? '0' + (date.getDate()) : date.getDate());
                var hh = (date.getHours() < 10 ? '0' + date.getHours() : date.getHours()) + ':';
                var mm = (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes()) + ':';
                var ss = (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds());
                td3.innerHTML = YY + MM + DD +" "+hh + mm + ss;
                tr.appendChild(td3);

                let td4 = document.createElement("td");
                let str2 = timelimit / 60;
                td4.innerHTML = str2;
                tr.appendChild(td4);
                
                examtable.appendChild(tr);
            }
        }
        else {
            let head = document.getElementById("examinfo");
            head.innerHTML = "暂无考试信息~";
            prompt4class("尚未加入班级，请输入您得班级号~");
        }
    }
    else if(data.infotype == "STUADDCLASS") {
        if(data.result == "SU") {
            alert("加入班级成功， 请刷新~");
        }
        else {
            prompt4class("加入班级失败，请输入正确的班级号~");
        }
    }
    else if(data.infotype == "LOADSTUTESTPAPER") {
        let selectpro = document.getElementById("stupaperselectpros");
        if(data.select.length > 0) {
            let headselectpro = document.createElement("h3");
            headselectpro.innerHTML = "选择题";
            selectpro.appendChild(headselectpro);
        }
        selectpronumber = data.select.length;
        localStorage.setItem("selectpronumber", selectpronumber);
        for(let i = 0; i < data.select.length; ++i) {
            let selectspan = document.createElement("span");
            let number = i + 1;
            selectspan.innerHTML = number.toString() + ". " + data.select[i].discription;
            selectpro.appendChild(selectspan);

            let selectname = number + "_select";
            let brA = document.createElement("br");
            selectpro.appendChild(brA);
            let selectspannbspA = document.createElement("span");
            selectspannbspA.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspA);
            let selectA = document.createElement("input");
            selectA.setAttribute("type", "radio");
            selectA.setAttribute("name", selectname);
            selectA.setAttribute("value", "A");
            selectpro.appendChild(selectA);
            let selectspanA = document.createElement("span");
            selectspanA.innerHTML = "&nbsp;&nbsp;A: " + data.select[i].A;
            selectpro.appendChild(selectspanA);

            let brB = document.createElement("br");
            selectpro.appendChild(brB);
            let selectspannbspB = document.createElement("span");
            selectspannbspB.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspB);
            let selectB = document.createElement("input");
            selectB.setAttribute("type", "radio");
            selectB.setAttribute("name", selectname);
            selectB.setAttribute("value", "B");
            selectpro.appendChild(selectB);
            let selectspanB = document.createElement("span");
            selectspanB.innerHTML = "&nbsp;&nbsp;B: " + data.select[i].B;
            selectpro.appendChild(selectspanB);

            let brC = document.createElement("br");
            selectpro.appendChild(brC);
            let selectspannbspC = document.createElement("span");
            selectspannbspC.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspC);
            let selectC = document.createElement("input");
            selectC.setAttribute("type", "radio");
            selectC.setAttribute("name", selectname);
            selectC.setAttribute("value", "C");
            selectpro.appendChild(selectC);
            let selectspanC = document.createElement("span");
            selectspanC.innerHTML = "&nbsp;&nbsp;C: " + data.select[i].C;
            selectpro.appendChild(selectspanC);

            let brD = document.createElement("br");
            selectpro.appendChild(brD);
            let selectspannbspD = document.createElement("span");
            selectspannbspD.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            selectpro.appendChild(selectspannbspD);
            let selectD = document.createElement("input");
            selectD.setAttribute("type", "radio");
            selectD.setAttribute("name", selectname);
            selectD.setAttribute("value", "D");
            selectpro.appendChild(selectD);
            let selectspanD = document.createElement("span");
            selectspanD.innerHTML = "&nbsp;&nbsp;D: " + data.select[i].D;
            selectpro.appendChild(selectspanD);

            let brE = document.createElement("br");
            selectpro.appendChild(brE);
        }

        let judgepro = document.getElementById("stupaperjudgepros");
        if(data.judge.length > 0) {
            let headjudgepro = document.createElement("h3");
            headjudgepro.innerHTML = "判断题";
            judgepro.appendChild(headjudgepro);
        }
        judgepronumber = data.judge.length;
        localStorage.setItem("judgepronumber", judgepronumber);
        for(let i = 0; i < data.judge.length; ++i) {
            let judgespan = document.createElement("span");
            let number = i + 1;
            judgespan.innerHTML = number.toString() + ". " + data.judge[i].discription;
            judgepro.appendChild(judgespan);

            let judgename = number + "_judge";
            let brA = document.createElement("br");
            judgepro.appendChild(brA);
            let judgespannbspA = document.createElement("span");
            judgespannbspA.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            judgepro.appendChild(judgespannbspA);
            let judgeA = document.createElement("input");
            judgeA.setAttribute("type", "radio");
            judgeA.setAttribute("name", judgename);
            judgeA.setAttribute("value", "A");
            judgepro.appendChild(judgeA);
            let judgespanA = document.createElement("span");
            judgespanA.innerHTML = "&nbsp;&nbsp;A: 正确";
            judgepro.appendChild(judgespanA);

            let brB = document.createElement("br");
            judgepro.appendChild(brB);
            let judgespannbspB = document.createElement("span");
            judgespannbspB.innerHTML = "&nbsp;&nbsp;&nbsp;&nbsp;";
            judgepro.appendChild(judgespannbspB);
            let judgeB = document.createElement("input");
            judgeB.setAttribute("type", "radio");
            judgeB.setAttribute("name", judgename);
            judgeB.setAttribute("value", "B");
            judgepro.appendChild(judgeB);
            let judgespanB = document.createElement("span");
            judgespanB.innerHTML = "&nbsp;&nbsp;B: 错误";
            judgepro.appendChild(judgespanB);

            let brcC = document.createElement("br");
            judgepro.appendChild(brcC);
        }

        let codepro = document.getElementById("stupapercodepros");
        tcodenumber = data.codelist.length;
        codepronumber = data.codelist.length;
        localStorage.setItem('tcodenumber', tcodenumber);
        localStorage.setItem('codepronumber', codepronumber);
        if(tcodenumber > 0) {
            let headcodepro = document.createElement("h3");
            headcodepro.innerHTML = "编程题";
            codepro.appendChild(headcodepro);
        }
        for(let i = 0; i < tcodenumber; ++i) {
            let br = document.createElement("br");
            codepro.appendChild(br);
            let title_a = document.createElement("a");
            title_a.setAttribute("href", "stucodepro.html");
            title_a.setAttribute("target", "htmlform");

            let fun = "load4teachertestpapercodepro(" + i + ")";
            title_a.setAttribute("onclick", fun);
            let number = i + 1;
            title_a.innerHTML = number + ". [" + data.codelist[i].name + "]";
            codepro.appendChild(title_a);
            let brr = document.createElement("br");
            codepro.appendChild(brr);
        }
    }
    else if(data.infotype == "SUBMITSTUTESTPAPERANSWER") {
        if(data.result == "SU") {
            alert("提交成功~");
        }
        else {
            alert("提交失败，请重试！");
        }
    }
    else if(data.infotype == "RUNSTUEXAMCODE") {
        let result = document.getElementById("runresult4exercise");
        if(data.status == 7) {
            result.innerHTML = "编译未通过：" + data.resultstr;
        }
        else if(data.status == 1) {
            result.innerHTML = "结果错误：" + data.resultstr;
        }
        else if(data.status == 2) {
            result.innerHTML = "运行超时：" + data.resultstr;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 0) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 8) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 1) {
            result.innerHTML = "完全正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory;
        }
    }
    else if(data.infotype == "SUBMITSTUEXAMCODE") {
        let result = document.getElementById("runresult4exercise");
        if(data.status == 7) {
            result.innerHTML = "编译未通过：" + data.resultstr;
        }
        else if(data.status == 1) {
            result.innerHTML = "结果错误：" + data.resultstr;
        }
        else if(data.status == 2) {
            result.innerHTML = "运行超时：" + data.resultstr;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 0) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 8) {
            result.innerHTML = "答案部分正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory + "\nratio: " + data.ratio;
        }
        else if(data.status == 0 && parseInt(data.ratio) == 1) {
            result.innerHTML = "完全正确：" + data.resultstr + "\ntime: " + data.time + "\nmemory: " + data.memory;
        }
        if(data.result == "SU") {
            alert("提交成功~");
        }
        else {
            alert("提交失败，请重试！");
        }
    }
    else if(data.infotype == "NEWCLASS") {
        if(data.result == "SU") {
            alert("创建成功，请刷新~");
        }
        else {
            alert("创建失败，请检查班级id是否存在！");
        }
    }
    else if(data.infotype == "DELETECLASS") {
        if(data.result == "SU") {
            alert("删除成功~");
        }
        else {
            alert("删除失败，请重试！");
        }
    }
    else if(data.infotype == "LOADTEACHERCLASS") {
        var testpapertable = document.getElementById("teacherclasses");
        for(let i = 0; i < data.classeslist.length; ++i) {
            var tr = document.createElement("tr");
            var td1 = document.createElement("td");;
            td1.innerHTML = data.classeslist[i].id;
            tr.appendChild(td1);
            var td2 = document.createElement("td");
            let str = "<a href=\"classes.html\" onclick=\"tload4ClassesInfo('" + data.classeslist[i].tusername + "', '" + data.classeslist[i].id + "')\" target=\"details\">[详情]</a>";
            td2.innerHTML = str;
            tr.appendChild(td2);
            var td3 = document.createElement("td");
            let str1 = "<a href=\"student.html\" onclick=\"tdelete4Classes('" + data.classeslist[i].tusername + "', '" + data.classeslist[i].id + "')\"  target=\"htmlform\">[删除]</a>";
            td3.innerHTML = str1;
            tr.appendChild(td3);
            testpapertable.appendChild(tr);
        }
    }
    else if(data.infotype == "LOADCLASSINFO") {
        let classname = localStorage.getItem('nowloadclassid');
        let classnames = document.getElementById("classname");
        classnames.innerHTML = classname + "班";
        let testpapertable = document.getElementById("classmember");
        for(let i = 0; i < data.classpeoplelist.length; ++i) {
            let tr = document.createElement("tr");
            let td1 = document.createElement("td");
            td1.innerHTML = data.classpeoplelist[i].material;
            tr.appendChild(td1);
            let td2 = document.createElement("td");
            td2.innerHTML = data.classpeoplelist[i].name;
            tr.appendChild(td2);
            let td3 = document.createElement("td");
            td3.innerHTML = data.classpeoplelist[i].username;
            tr.appendChild(td3);
            let td4 = document.createElement("td");
            let str = "<a href=\"classes.html\" onclick=\"deleteclassmember('" + data.classpeoplelist[i].username + "')\" target=\"details\">[删除]</a>";
            td4.innerHTML = str;
            tr.appendChild(td4);
            testpapertable.appendChild(tr);
        }
        let tr = document.createElement("tr");
        let td1 = document.createElement("td");
        td1.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newmembermaterial\">"
        tr.appendChild(td1);
        let td2 = document.createElement("td");
        td2.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newmembername\">"
        tr.appendChild(td2);
        let td3 = document.createElement("td");
        td3.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newmemberusername\">"
        tr.appendChild(td3);
        let td4 = document.createElement("td");
        let str = "<a href=\"classes.html\" onclick=\"addclassmember()\" target=\"details\">[添加成员]</a>";
        td4.innerHTML = str;
        tr.appendChild(td4);
        testpapertable.appendChild(tr);
    }
    else if(data.infotype == "DELETECLASSMEMBER") {
        if(data.result == "SU") {
            alert("删除成员成功~");
        }
        else {
            alert("删除成员失败，请重试！");
        }
    }
    else if(data.infotype == "ADDCLASSMEMBER") {
        if(data.result == "SU") {
            alert("添加成功，初始密码：用户名+12345678");
        }
        else {
            alert("添加失败，请重试（若用户已存在，请通知主动加入班级）！");
        }
    }
    else if(data.infotype == "LOADACHIEVEMENTCLASS") {
        let select4classidac = document.getElementById("select4classidac");
        for(let i = 0; i < data.classeslist.length; ++i) {
            var option = document.createElement("option");
            option.innerHTML = data.classeslist[i].classid;
            select4classidac.appendChild(option);
        }
    }
    else if(data.infotype == "LOADACHIEVEMENT") {
        nowloadtestpaperselectnumber = localStorage.getItem('nowloadtestpaperselectnumber');
        nowloadtestpaperjudgenumber = localStorage.getItem('nowloadtestpaperjudgenumber');
        nowloadtestpapercodenumber = localStorage.getItem('nowloadtestpapercodenumber');
        let testpapertable = document.getElementById("achievementtable");
        let len = testpapertable.rows.length;
        for(var i = 0; i < len; i++) {
            testpapertable.deleteRow(0);
        }
        let htr = document.createElement("tr");
        let th1 = document.createElement("th");
        th1.setAttribute("width", "100px");
        th1.innerHTML = "用户名";
        htr.appendChild(th1);
        let th2 = document.createElement("th");
        th2.setAttribute("width", "100px");
        th2.innerHTML = "姓名";
        htr.appendChild(th2);
        let th3 = document.createElement("th");
        th3.setAttribute("width", "100px");
        th3.innerHTML = "学号";
        htr.appendChild(th3);
        for(let i = 1; i <= parseInt(nowloadtestpaperselectnumber) + parseInt(nowloadtestpaperjudgenumber) + parseInt(nowloadtestpapercodenumber); ++i) {
            let th = document.createElement("th");
            th.setAttribute("width", "120px");
            th.innerHTML = i + ".[得分:答案]";
            htr.appendChild(th);
        }
        let th4 = document.createElement("th");
        th4.setAttribute("width", "120px");
        th4.innerHTML = "总分";
        htr.appendChild(th4);
        testpapertable.appendChild(htr);
        let totleac = 0.00;
        for(let i = 0; i < data.achievement.length; ++i) {
            let tr = document.createElement("tr");
            let td1 = document.createElement("td");
            td1.innerHTML = data.achievement[i].username;
            tr.appendChild(td1);
            let td2 = document.createElement("td");
            td2.innerHTML = data.achievement[i].name;
            tr.appendChild(td2);
            let td3 = document.createElement("td");
            td3.innerHTML = data.achievement[i].material;
            tr.appendChild(td3);
            for(let j = 1; j <= nowloadtestpaperselectnumber; ++j) {
                let name4select = j + "_select";
                let td = document.createElement("td");
                let str = data.achievement[i][name4select];
                let words = str.split("[&]");
                totleac += parseFloat(words[1]);
                td.innerHTML = "[" + words[1] + " : " + words[0] + "]";
                tr.appendChild(td);
            }
            for(let j = 1; j <= nowloadtestpaperjudgenumber; ++j) {
                let name4judge = j + "_judge";
                let td = document.createElement("td");
                let str = data.achievement[i][name4judge];
                let words = str.split("[&]");
                totleac += parseFloat(words[1]);
                td.innerHTML = "[" + words[1] + " : " + words[0] + "]";
                tr.appendChild(td);
            }
            for(let j = 1; j <= nowloadtestpapercodenumber; ++j) {
                let name4code = j + "_code";
                let td = document.createElement("td");
                let str = data.achievement[i][name4code];
                let words = str.split("[&]");
                totleac += parseFloat(words[1]);
                let str1 = words[0];
                td.innerHTML = "[" + parseFloat(words[1]).toFixed(2) + " : " + "<a href=\"#\" onclick=\"lookcode('" + str1 + "')\">查看代码</a>" + "]";
                tr.appendChild(td);
            }
            let td4 = document.createElement("td");
            td4.innerHTML = totleac.toFixed(2);
            tr.appendChild(td4);
            testpapertable.appendChild(tr);
        }
    }
    else if(data.infotype == "RLOADTEACHER") {
        let testpapertable = document.getElementById("rteachertable");
        for(let i = 0; i < data.teacherlist.length; ++i) {
            let tr = document.createElement("tr");
            let td1 = document.createElement("td");
            td1.innerHTML = data.teacherlist[i].username;
            tr.appendChild(td1);
            let td2 = document.createElement("td");
            td2.innerHTML = data.teacherlist[i].name;
            tr.appendChild(td2);
            let td3 = document.createElement("td");
            td3.innerHTML = data.teacherlist[i].material;
            tr.appendChild(td3);
            let td4 = document.createElement("td");
            let str = "<a href=\"teacher.html\" onclick=\"rdeleteteacher('" + data.teacherlist[i].username + "')\" target=\"htmlform\">[删除]</a>";
            td4.innerHTML = str;
            tr.appendChild(td4);
            testpapertable.appendChild(tr);
        }
        let tr = document.createElement("tr");
        let td1 = document.createElement("td");
        td1.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newrusername\">"
        tr.appendChild(td1);
        let td2 = document.createElement("td");
        td2.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newrname\">"
        tr.appendChild(td2);
        let td3 = document.createElement("td");
        td3.innerHTML = "<input class=\"examinput\" style=\"width: 100px;\" id=\"newrmaterial\">"
        tr.appendChild(td3);
        let td4 = document.createElement("td");
        let str = "<a href=\"teacher.html\" onclick=\"addrteacher()\" target=\"htmlform\">[添加成员]</a>";
        td4.innerHTML = str;
        tr.appendChild(td4);
        testpapertable.appendChild(tr);
    }
    else if(data.infotype == "RDELETETEACHER") {
        if(data.result == "SU") {
            alert("删除成功~");
        }
        else {
            alert("删除失败，请重试！");
        }
    }
    else if(data.infotype == "ADDRTEACHER") {
        if(data.result == "SU") {
            alert("添加成功，初始密码：用户名+12345678");
        }
        else {
            alert("添加失败，请重试（若用户已存在，请通知主动加入班级）！");
        }
    }
    else if(data.infotype == "EXECUTEROOTCODE") {
        let rresult = document.getElementById("resultcode");
        if(data.result == "SU") {
            let arg = data.execresult;
            arg = arg.replace(/~n/g, "\\n");
            arg = arg.replace(/~/g, "\n");
            arg = arg.replace(/`/g, "\t");
            rresult.innerHTML = arg;
        }
        else {
            rresult.innerHTML = data.error;
        }
    }
}
// --------------------------------------------------------------------------

// 登录 ---------------------------------------------------------------------
let loginusername;
let loginpasswd;

function login() {
    localStorage.setItem('islogin', false);
	loginusername = document.getElementById("loginusername").value;
    loginpasswd = document.getElementById("loginpassword").value;
	if(loginpasswd.length < 8 || loginusername.length < 4) {
	    alert('🥺用户名或密码不规范！🥺');
	}
    else {
		var arg = "{\"infotype\":\"LOGIN\", \"username\":\"" + loginusername +  "\", \"password\":\"" + loginpasswd + "\"}";
        sendRequest(arg);
	}
}
// --------------------------------------------------------------------------

// 注册 ---------------------------------------------------------------------
function logup() {
	let logupusername = document.getElementById("logupusername").value;
    let loguppasswd = document.getElementById("loguppassword").value;
    let logupsure = document.getElementById("logupsure").value;
    let logupusertype = document.getElementById("logupusertype").selectedIndex;
    let logupname = document.getElementById("logupname").value;
    let logupmaterial = document.getElementById("logupmaterial").value;

	if(loguppasswd.length < 8 || logupusername.length < 4) {
	    alert('🥺用户名或密码不规范！🥺');
	}
    else if(logupname == "" || logupmaterial == "") {
	    alert('😶请完善相关信息！😶');
	}
    else if(loguppasswd != logupsure) {
        alert('🙈两次密码不一致！🙈');
    }
    else {
		var arg = "{\"infotype\":\"LOGUP\", \"username\":\"" + logupusername+  "\", \
                    \"password\":\"" + loguppasswd + "\", \
                    \"usertype\":\"" + logupusertype + "\", \
                    \"name\":\"" + logupname + "\", \
                    \"material\":\"" + logupmaterial + "\"}";
        alert(arg);
        sendRequest(arg);
	}
}
// --------------------------------------------------------------------------

// 加载 ---------------------------------------------------------------------
function load4stu() {
    islogin = localStorage.getItem('islogin');
    if(islogin == "true") {
        var aObj = document.getElementById("username4stu");
        username = localStorage.getItem('username');
        passwd = localStorage.getItem('password');
        usertype = localStorage.getItem('usertype');
        aObj.innerText = username;
    }
    else {
        alert("🐾请先登录！🐾");
        window.location.href = "../index.html";
    }
}
// --------------------------------------------------------------------------

// 退出 ---------------------------------------------------------------------
function quit() {
    localStorage.setItem('islogin', false);
    localStorage.setItem('username', "");
    localStorage.setItem('password', "");
    localStorage.setItem('usertype', "");
    localStorage.setItem('name', "");
    localStorage.setItem('number4exercise', "");
    localStorage.setItem('material', "");
    localStorage.setItem('classid', "");
    localStorage.setItem('tpid', "");
    localStorage.setItem('tpname', "");
    islogin = false;
}
// --------------------------------------------------------------------------

// exercise -----------------------------------------------------------------
function load4exerciselist() {
    let username = localStorage.getItem('username');
	let arg = "{\"infotype\":\"EXERCISELIST\", \"username\":\"" + username + "\", \"pages\":\"" + page4exercise + "\"}";
    sendRequest(arg);
}

function afterpage() {
    if(page4exercise == 0) {
        alert("没有上一页了！");
        return;
    }
    page4exercise--;
    let username = localStorage.getItem('username');
    let arg = "{\"infotype\":\"EXERCISELIST\", \"username\":\"" + username + "\", \"pages\":\"" + page4exercise + "\"}";
    sendRequest(arg);
}

function nextpage() {
    let number4exercise = localStorage.getItem('number4exercise');
    if(page4exercise == parseInt(number4exercise / 20)) {
        alert("没有下一页了！");
        return;
    }
    page4exercise++;
    let username = localStorage.getItem('username');
    let arg = "{\"infotype\":\"EXERCISELIST\", \"username\":\"" + username + "\", \"pages\":\"" + page4exercise + "\"}";
    sendRequest(arg);
}

function jumppage() {
    let number4exercise = localStorage.getItem('number4exercise');
    let page = document.getElementById("change4exercise").value - 1;
    if(page >= 0 && page <= parseInt(number4exercise / 20)) {
        page4exercise = page;
        let username = localStorage.getItem('username');
        let arg = "{\"infotype\":\"EXERCISELIST\", \"username\":\"" + username + "\", \"pages\":\"" + page4exercise + "\"}";
        sendRequest(arg);
    }
    else {
        alert("无效的页数！");
    }
}

function loadexerpro(obj) {
    nowexerpro = obj;
    localStorage.setItem('nowexerpro', obj);
}

function load4exerpro() {
    let obj = localStorage.getItem('nowexerpro');
    nowexerpro = obj;
    number4exercise = localStorage.getItem('number4exercise');
    var arg = "{\"infotype\":\"LOADEXERPRO\", \"id\":\"" + obj + "\"}";
    sendRequest(arg);
}

function prexproblem4exercise() {
    if(nowexerpro == 1) {
        alert("没有上一题了！");
        return;
    }
    nowexerpro--;
    var arg = "{\"infotype\":\"LOADEXERPRO\", \"id\":\"" + nowexerpro + "\"}";
    sendRequest(arg);
}

function nextproblem4exercise() {
    if(number4exercise == nowexerpro) {
        alert("没有下一题了！");
        return;
    }
    nowexerpro++;
    var arg = "{\"infotype\":\"LOADEXERPRO\", \"id\":\"" + nowexerpro + "\"}";
    sendRequest(arg);
}

function submitcode4exercise() {
    let answercode4exercise = document.getElementById("answercode4exercise").value;
    if(answercode4exercise == "") {
        alert("请在代码框输入你的代码~");
        return;
    }
    answercode4exercise = answercode4exercise.replace(/"/g,"\\\"");
    alert(answercode4exercise);
    let username4submitcode = localStorage.getItem('username');
    let type4exercise = "C";
    if(document.getElementById("type4exercise").value == 0) {
        type4exercise = "C";
    }
    else if(document.getElementById("type4exercise").value == 1) {
        type4exercise = "C++";
    }
    var arg = "{\"infotype\":\"SUBMITCODE\", \"fname\":\"exercise_" + nowexerpro + "\", \"qname\":\"" + username4submitcode + "\", \"type\":\"" + type4exercise + "\", \"id\":\"" + nowexerpro + "\", \"code\":\"" + answercode4exercise + "\"}";
    sendRequest(arg);
}

function runcode4exercise() {
    let answercode4exercise = document.getElementById("answercode4exercise").value;
    if(answercode4exercise == "") {
        alert("请在代码框输入你的代码~");
        return;
    }
    answercode4exercise = answercode4exercise.replace(/"/g,"\\\"");
    alert(answercode4exercise);
    let username4submitcode = localStorage.getItem('username');
    let type4exercise = "C";
    if(document.getElementById("type4exercise").value == 0) {
        type4exercise = "C";
    }
    else if(document.getElementById("type4exercise").value == 1) {
        type4exercise = "C++";
    }
    var arg = "{\"infotype\":\"RUNCODE\", \"fname\":\"exercise_" + nowexerpro + "\", \"qname\":\"" + username4submitcode + "\", \"type\":\"" + type4exercise + "\", \"id\":\"" + nowexerpro + "\", \"code\":\"" + answercode4exercise + "\"}";
    sendRequest(arg);
}
// --------------------------------------------------------------------------

// 试卷 ---------------------------------------------------------------------
function load4TestPaper() {
    username = localStorage.getItem('username');
    var arg = "{\"infotype\":\"LOADTESTPAPER\", \"username\":\"" + username + "\"}";
    sendRequest(arg);
}

function ReaderFiles(str, flag) {
    var selectedFile = document.getElementById(str).files[0];    //获取读取的File对象
    if(undefined == selectedFile) {
        return;
    }
    else {
        var reader = new FileReader();      // 这里是核心！！！读取操作就是由它完成的。
        reader.readAsText(selectedFile);    // 读取文件的内容
        if(flag == "in_1") {
            reader.onload = function() {
                file_in_1_content = this.result;
            };
        }
        else if(flag == "in_2") {
            reader.onload = function() {
                file_in_2_content = this.result;
            };
        }
        else if(flag == "in_3") {
            reader.onload = function() {
                file_in_3_content = this.result;
            };
        }
        else if(flag == "out_1") {
            reader.onload = function() {
                file_out_1_content = this.result;
            };
        }
        else if(flag == "out_2") {
            reader.onload = function() {
                file_out_2_content = this.result;
            };
        }
        else if(flag == "out_3") {
            reader.onload = function() {
                file_out_3_content = this.result;
            };
        }
    }
 }

var issubmit = false;

function createxam() {
	var name4exam = document.getElementById("name4exam").value;
	var number4select = document.getElementById("selectpro").value;
	var number4judge = document.getElementById("judgepro").value;
	var number4code = document.getElementById("codepro").value;
	
	var score4select = document.getElementById("score4selectpro").value;
	var score4judge = document.getElementById("score4judgepro").value;
	
	if(name4exam == "") {
		// alert("题目名称无效！");
		// return;
	}
	if(number4code < 0 || number4judge < 0 || number4select < 0 || number4code > 100 || number4judge > 100 || number4select > 100
		|| score4select < 0 || score4judge < 0
		|| number4code == "" || number4judge == "" || number4select == "" || score4select == "" || score4judge == "") {
		// alert("题型数量/分值无效！");
		// return;
	}
	
	var selectproblem = document.getElementById("selectproblem");
	var judgeproblem = document.getElementById("judegproblem");
	var codeproblem = document.getElementById("codeproblem");
	
	for(var i = 0; i < number4select; ++i) {
		var son1 = document.createElement("span");
		var str1 = "<br>";
		son1.innerHTML = str1;
		selectproblem.appendChild(son1);
		
		var son2 = document.createElement("textarea");
		son2.setAttribute("class", "problemdescribe");
		var str2 = "describe_select_" + (i + 1);
		son2.setAttribute("id", str2);
		var str2 = (i + 1) + ". 题目描述：";
		son2.setAttribute("placeholder", str2);
		selectproblem.appendChild(son2);
		
		var son3 = document.createElement("span");
		var str3 = "<br>&nbsp;&nbsp;&nbsp;设置选项：A：";
		son3.innerHTML = str3;
		selectproblem.appendChild(son3);
		var son4 = document.createElement("input");
		son4.setAttribute("class", "selectdescribe");
		var str4 = "A_" + (i + 1);
		son4.setAttribute("id", str4);
		selectproblem.appendChild(son4);
		
		var son5 = document.createElement("span");
		son5.innerHTML = "B：";
		selectproblem.appendChild(son5);
		var son6 = document.createElement("input");
		son6.setAttribute("class", "selectdescribe");
		var str6 = "B_" + (i + 1);
		son6.setAttribute("id", str6);
		selectproblem.appendChild(son6);
		
		var son7 = document.createElement("span");
		son7.innerHTML = "C：";
		selectproblem.appendChild(son7);
		var son8 = document.createElement("input");
		son8.setAttribute("class", "selectdescribe");
		var str8 = "C_" + (i + 1);
		son8.setAttribute("id", str8);
		selectproblem.appendChild(son8);
		
		var son9 = document.createElement("span");
		son9.innerHTML = "D：";
		selectproblem.appendChild(son9);
		var son10 = document.createElement("input");
		son10.setAttribute("class", "selectdescribe");
		var str10 = "D_" + (i + 1);
		son10.setAttribute("id", str10);
		selectproblem.appendChild(son10);
		
		var son11 = document.createElement("span");
		son11.innerHTML = "&nbsp;&nbsp;答案：";
		selectproblem.appendChild(son11);
		var son12 = document.createElement("select");
		var str12 = "answer_select_" + (i + 1);
		son12.setAttribute("id", str12);
		son12.options.add(new Option("A", "A"));
		son12.options.add(new Option("B", "B"));
		son12.options.add(new Option("C", "C"));
		son12.options.add(new Option("D", "D"));
		selectproblem.appendChild(son12);
	}
	
	for(var j = 0; j < number4judge; ++j) {
		var son1 = document.createElement("span");
		var str1 = "<br>";
		son1.innerHTML = str1;
		judgeproblem.appendChild(son1);
		
		var son2 = document.createElement("textarea");
		son2.setAttribute("class", "problemdescribe");
		var str2 = "describe_judge_" + (j + 1);
		son2.setAttribute("id", str2);
		var str2 = (j + 1) + ". 题目描述：";
		son2.setAttribute("placeholder", str2);
		judgeproblem.appendChild(son2);
		
		var son3 = document.createElement("span");
		son3.innerHTML = "<br>&nbsp;&nbsp;&nbsp;答案：";
		judgeproblem.appendChild(son3);
		var son4 = document.createElement("select");
		var str4 = "answer_judge_" + (j + 1);
		son4.setAttribute("id", str4);
		son4.options.add(new Option("正确", "A"));
		son4.options.add(new Option("错误", "B"));
		judgeproblem.appendChild(son4);
	}
	
	for(var k = 0; k < number4code; ++k) {
		var son_1 = document.createElement("span");
		var str_1 = "<br><br>" + (k + 1) + ". 题目名称：";
		son_1.innerHTML = str_1;
		codeproblem.appendChild(son_1);
		
		var son_2 = document.createElement("input");
		son_2.setAttribute("class", "problemdescribe");
		var str_2 = "name_code_" + (k + 1);
		son_2.setAttribute("id", str_2);
		son_2.setAttribute("class", "examinput");
		son_2.setAttribute("style", "width: 300px;");
		codeproblem.appendChild(son_2);
		
		var son_s = document.createElement("span");
		var str_s = "&nbsp;&nbsp;&nbsp;&nbsp;分值：";
		son_s.innerHTML = str_s;
		codeproblem.appendChild(son_s);
		
		var son_ss = document.createElement("input");
		son_ss.setAttribute("class", "problemdescribe");
		var str_ss = "score_code_" + (k + 1);
		son_ss.setAttribute("id", str_ss);
		son_ss.setAttribute("class", "examinput");
		son_ss.setAttribute("type", "number");
		son_ss.setAttribute("style", "width: 150px;");
		codeproblem.appendChild(son_ss);
		
		var son2 = document.createElement("textarea");
		son2.setAttribute("class", "problemdescribe");
		var str2 = "describe_code_" + (k + 1);
		son2.setAttribute("id", str2);
		son2.setAttribute("placeholder", "题目描述");
		codeproblem.appendChild(son2);
		
		var son_4 = document.createElement("textarea");
		son_4.setAttribute("class", "problemdescribe");
		var str_4 = "explain_" + (k + 1);
		son_4.setAttribute("id", str_4);
		son_4.setAttribute("placeholder", "题目说明");
		codeproblem.appendChild(son_4);
		
		var son_6 = document.createElement("textarea");
		son_6.setAttribute("class", "problemdescribe");
		son_6.setAttribute("style", "width: 30%;");
		var str_6 = "example_1_" + (k + 1);
		son_6.setAttribute("id", str_6);
		son_6.setAttribute("placeholder", "示例1");
		codeproblem.appendChild(son_6);
		
		var son_8 = document.createElement("textarea");
		son_8.setAttribute("class", "problemdescribe");
		son_8.setAttribute("style", "width: 30%;");
		var str_8 = "example_2_" + (k + 1);
		son_8.setAttribute("id", str_8);
		son_8.setAttribute("placeholder", "示例2");
		codeproblem.appendChild(son_8);
		
		var son3 = document.createElement("span");
		var str3 = "<br>&nbsp;&nbsp;&nbsp;要求：时间限制(ms)：";
		son3.innerHTML = str3;
		codeproblem.appendChild(son3);
		var son4 = document.createElement("input");
		son4.setAttribute("class", "examinput");
		son4.setAttribute("type", "number");
		var str4 = "times_" + (k + 1);
		son4.setAttribute("id", str4);
		codeproblem.appendChild(son4);
		
		var son5 = document.createElement("span");
		son5.innerHTML = "内存限制(b)：";
		codeproblem.appendChild(son5);
		var son6 = document.createElement("input");
		son6.setAttribute("class", "examinput");
		son6.setAttribute("type", "number");
		var str6 = "memory_" + (k + 1);
		son6.setAttribute("id", str6);
		codeproblem.appendChild(son6);
		
		var son7 = document.createElement("span");
		son7.innerHTML = "<br><br>&nbsp;&nbsp;&nbsp;输入文件：";
		codeproblem.appendChild(son7);
		var son7 = document.createElement("input");
		son7.setAttribute("type", "file");
		var str7 = "file_in_1_" + (k + 1);
		son7.setAttribute("id", str7);
		codeproblem.appendChild(son7);
		var son8 = document.createElement("input");
		son8.setAttribute("type", "file");
		var str8 = "file_in_2_" + (k + 1);
		son8.setAttribute("id", str8);
		codeproblem.appendChild(son8);
		var son9 = document.createElement("input");
		son9.setAttribute("type", "file");
		var str9 = "file_in_3_" + (k + 1);
		son9.setAttribute("id", str9);
		codeproblem.appendChild(son9);
		
		var son10 = document.createElement("span");
		son10.innerHTML = "<br><br>&nbsp;&nbsp;&nbsp;输出文件：";
		codeproblem.appendChild(son10);
		var son11 = document.createElement("input");
		son11.setAttribute("type", "file");
		var str11 = "file_out_1_" + (k + 1);
		son11.setAttribute("id", str11);
		codeproblem.appendChild(son11);
		var son12 = document.createElement("input");
		son12.setAttribute("type", "file");
		var str12 = "file_out_2_" + (k + 1);
		son12.setAttribute("id", str12);
		codeproblem.appendChild(son12);
		var son13 = document.createElement("input");
		son13.setAttribute("type", "file");
		var str13 = "file_out_3_" + (k + 1);
		son13.setAttribute("id", str13);
		codeproblem.appendChild(son13);
	}
}

function submitexam() {
    if(issubmit == false) {
        issubmit = true;
        let examname = document.getElementById("name4exam").value;
        username = localStorage.getItem('username');
        var arg = "{\"infotype\":\"SUBMITEXAM\", \"username\": \"" + username + "\", \"name\": \"" + examname + "\", \"select\": [";
        let selectpro = document.getElementById("selectpro").value;
        let score4selectpro = document.getElementById("score4selectpro").value;
        let judgepro = document.getElementById("judgepro").value;
        let score4judgepro = document.getElementById("score4judgepro").value;
        let codepro = document.getElementById("codepro").value;

        for(let i = 0; i < selectpro; ++i) {
            let str2 = "describe_select_" + (i + 1);
            let describle = document.getElementById(str2).value;
            let str_A = "A_" + (i + 1);
            let str_B = "B_" + (i + 1);
            let str_C = "C_" + (i + 1);
            let str_D = "D_" + (i + 1);
            let option_A = document.getElementById(str_A).value;
            let option_B = document.getElementById(str_B).value;
            let option_C = document.getElementById(str_C).value;
            let option_D = document.getElementById(str_D).value;
            let str3 = "answer_select_" + (i + 1);
            let answer = document.getElementById(str3).value;
            if(i == selectpro - 1) {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"0\", "
                    + "\"option\": \"" + option_A + "[&]" + option_B + "[&]" + option_C + "[&]" + option_D + "\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4selectpro + "\", "
                    + "\"inumber\": \"0\"}";
            }
            else {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"0\", "
                    + "\"option\": \"" + option_A + "[&]" + option_B + "[&]" + option_C + "[&]" + option_D + "\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4selectpro + "\", "
                    + "\"inumber\": \"0\"}, ";
            }
        
        }
        arg += "], \"judge\": [";

        for(let i = 0; i < judgepro; ++i) {
            let str2 = "describe_judge_" + (i + 1);
            let describle = document.getElementById(str2).value;
            let str3 = "answer_judge_" + (i + 1);
            let answer = document.getElementById(str3).value;
            if(i == judgepro - 1) {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"1\", "
                    + "\"option\": \"正确[&]错误\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4judgepro + "\", "
                    + "\"inumber\": \"0\"}";
            }
            else {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"1\", "
                    + "\"option\": \"正确[&]错误\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4judgepro + "\", "
                    + "\"inumber\": \"0\"}, ";
            }
        
        }
        arg += "], \"code\": [";
        for(let i = 0; i < codepro; ++i) {
            let str2 = "name_code_" + (i + 1);
            let name = document.getElementById(str2).value;
            let str3 = "score_code_" + (i + 1);
            let score = document.getElementById(str3).value;
            let str4 = "describe_code_" + (i + 1);
            let description = document.getElementById(str4).value;
            let str5 = "explain_" + (i + 1);
            let explain = document.getElementById(str5).value;
            let str6 = "example_1_" + (i + 1);
            let examples1 = document.getElementById(str6).value;
            let str7 = "example_2_" + (i + 1);
            let examples2 = document.getElementById(str7).value;
            let str8 = "times_" + (i + 1);
            let timelimit = document.getElementById(str8).value;
            let str9 = "memory_" + (i + 1);
            let memorylimit = document.getElementById(str9).value;

            let str10 = "file_in_1_" + (i + 1);
            ReaderFiles(str10, "in_1");
            let str11 = "file_in_2_" + (i + 1);
            ReaderFiles(str11, "in_2");
            let str12 = "file_in_3_" + (i + 1);
            ReaderFiles(str12, "in_3");

            let str13 = "file_out_1_" + (i + 1);
            ReaderFiles(str13, "out_1");
            let str14 = "file_out_2_" + (i + 1);
            ReaderFiles(str14, "out_2");
            let str15 = "file_out_3_" + (i + 1);
            ReaderFiles(str15, "out_3");

            if(i == codepro - 1) {
                arg += "{\"name\": \"" + name + "\", "
                    + "\"score\": \"" + score + "\", "
                    + "\"description\": \"" + description + "\", "
                    + "\"explain\": \"" + explain + "\", "
                    + "\"examples1\": \"" + examples1 + "\", "
                    + "\"examples2\": \"" + examples2 + "\", "
                    + "\"timelimit\": \"" + timelimit + "\", "
                    + "\"memorylimit\": \"" + memorylimit + "\", "
                    + "\"file_in_1\": \"" + file_in_1_content + "\", "
                    + "\"file_in_2\": \"" + file_in_2_content + "\", "
                    + "\"file_in_3\": \"" + file_in_3_content + "\", "
                    + "\"file_out_1\": \"" + file_out_1_content + "\", "
                    + "\"file_out_2\": \"" + file_out_2_content + "\", "
                    + "\"file_out_3\": \"" + file_out_3_content + "\", "
                    + "\"inumber\": \"3\"}";
            }
            else {
                arg += "{\"name\": \"" + name + "\", "
                    + "\"score\": \"" + score + "\", "
                    + "\"description\": \"" + description + "\", "
                    + "\"explain\": \"" + explain + "\", "
                    + "\"examples1\": \"" + examples1 + "\", "
                    + "\"examples2\": \"" + examples2 + "\", "
                    + "\"timelimit\": \"" + timelimit + "\", "
                    + "\"memorylimit\": \"" + memorylimit + "\", "
                    + "\"file_in_1\": \"" + file_in_1_content + "\", "
                    + "\"file_in_2\": \"" + file_in_2_content + "\", "
                    + "\"file_in_3\": \"" + file_in_3_content + "\", "
                    + "\"file_out_1\": \"" + file_out_1_content + "\", "
                    + "\"file_out_2\": \"" + file_out_2_content + "\", "
                    + "\"file_out_3\": \"" + file_out_3_content + "\", "
                    + "\"inumber\": \"3\"}, ";
            }
        
        }
        arg += "]}";
        alert("检测文件完成，请重新提交~");
    }
    else {
        let examname = document.getElementById("name4exam").value;
        username = localStorage.getItem('username');
        var arg = "{\"infotype\":\"SUBMITEXAM\", \"username\": \"" + username + "\", \"name\": \"" + examname + "\", \"select\": [";
        let selectpro = document.getElementById("selectpro").value;
        let score4selectpro = document.getElementById("score4selectpro").value;
        let judgepro = document.getElementById("judgepro").value;
        let score4judgepro = document.getElementById("score4judgepro").value;
        let codepro = document.getElementById("codepro").value;

        for(let i = 0; i < selectpro; ++i) {
            let str2 = "describe_select_" + (i + 1);
            let describle = document.getElementById(str2).value;
            let str_A = "A_" + (i + 1);
            let str_B = "B_" + (i + 1);
            let str_C = "C_" + (i + 1);
            let str_D = "D_" + (i + 1);
            let option_A = document.getElementById(str_A).value;
            let option_B = document.getElementById(str_B).value;
            let option_C = document.getElementById(str_C).value;
            let option_D = document.getElementById(str_D).value;
            let str3 = "answer_select_" + (i + 1);
            let answer = document.getElementById(str3).value;
            if(i == selectpro - 1) {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"0\", "
                    + "\"option\": \"" + option_A + "[&]" + option_B + "[&]" + option_C + "[&]" + option_D + "\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4selectpro + "\", "
                    + "\"inumber\": \"0\"}";
            }
            else {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"0\", "
                    + "\"option\": \"" + option_A + "[&]" + option_B + "[&]" + option_C + "[&]" + option_D + "\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4selectpro + "\", "
                    + "\"inumber\": \"0\"}, ";
            }
        
        }
        arg += "], \"judge\": [";

        for(let i = 0; i < judgepro; ++i) {
            let str2 = "describe_judge_" + (i + 1);
            let describle = document.getElementById(str2).value;
            let str3 = "answer_judge_" + (i + 1);
            let answer = document.getElementById(str3).value;
            if(i == judgepro - 1) {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"1\", "
                    + "\"option\": \"正确[&]错误\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4judgepro + "\", "
                    + "\"inumber\": \"0\"}";
            }
            else {
                arg += "{\"discription\": \"" + describle + "\", "
                    + "\"type\": \"1\", "
                    + "\"option\": \"正确[&]错误\", "
                    + "\"answer\": \"" + answer + "\", "
                    + "\"score\": \"" + score4judgepro + "\", "
                    + "\"inumber\": \"0\"}, ";
            }
        
        }
        arg += "], \"code\": [";
        for(let i = 0; i < codepro; ++i) {
            let str2 = "name_code_" + (i + 1);
            let name = document.getElementById(str2).value;
            let str3 = "score_code_" + (i + 1);
            let score = document.getElementById(str3).value;
            let str4 = "describe_code_" + (i + 1);
            let description = document.getElementById(str4).value;
            let str5 = "explain_" + (i + 1);
            let explain = document.getElementById(str5).value;
            let str6 = "example_1_" + (i + 1);
            let examples1 = document.getElementById(str6).value;
            let str7 = "example_2_" + (i + 1);
            let examples2 = document.getElementById(str7).value;
            let str8 = "times_" + (i + 1);
            let timelimit = document.getElementById(str8).value;
            let str9 = "memory_" + (i + 1);
            let memorylimit = document.getElementById(str9).value;

            let str10 = "file_in_1_" + (i + 1);
            ReaderFiles(str10, "in_1");
            let str11 = "file_in_2_" + (i + 1);
            ReaderFiles(str11, "in_2");
            let str12 = "file_in_3_" + (i + 1);
            ReaderFiles(str12, "in_3");

            let str13 = "file_out_1_" + (i + 1);
            ReaderFiles(str13, "out_1");
            let str14 = "file_out_2_" + (i + 1);
            ReaderFiles(str14, "out_2");
            let str15 = "file_out_3_" + (i + 1);
            ReaderFiles(str15, "out_3");

            if(i == judgepro - 1) {
                arg += "{\"name\": \"" + name + "\", "
                    + "\"score\": \"" + score + "\", "
                    + "\"description\": \"" + description + "\", "
                    + "\"explain\": \"" + explain + "\", "
                    + "\"examples1\": \"" + examples1 + "\", "
                    + "\"examples2\": \"" + examples2 + "\", "
                    + "\"timelimit\": \"" + timelimit + "\", "
                    + "\"memorylimit\": \"" + memorylimit + "\", "
                    + "\"file_in_1\": \"" + file_in_1_content + "\", "
                    + "\"file_in_2\": \"" + file_in_2_content + "\", "
                    + "\"file_in_3\": \"" + file_in_3_content + "\", "
                    + "\"file_out_1\": \"" + file_out_1_content + "\", "
                    + "\"file_out_2\": \"" + file_out_2_content + "\", "
                    + "\"file_out_3\": \"" + file_out_3_content + "\", "
                    + "\"inumber\": \"3\"}";
            }
            else {
                arg += "{\"name\": \"" + name + "\", "
                    + "\"score\": \"" + score + "\", "
                    + "\"description\": \"" + description + "\", "
                    + "\"explain\": \"" + explain + "\", "
                    + "\"examples1\": \"" + examples1 + "\", "
                    + "\"examples2\": \"" + examples2 + "\", "
                    + "\"timelimit\": \"" + timelimit + "\", "
                    + "\"memorylimit\": \"" + memorylimit + "\", "
                    + "\"file_in_1\": \"" + file_in_1_content + "\", "
                    + "\"file_in_2\": \"" + file_in_2_content + "\", "
                    + "\"file_in_3\": \"" + file_in_3_content + "\", "
                    + "\"file_out_1\": \"" + file_out_1_content + "\", "
                    + "\"file_out_2\": \"" + file_out_2_content + "\", "
                    + "\"file_out_3\": \"" + file_out_3_content + "\", "
                    + "\"inumber\": \"3\"}, ";
            }
        
        }
        arg += "]}";
        sendRequest(arg);
    }
}

function load4TestPaperPro(args, testpapername) {
    tpname = testpapername;
    tpid = args;
    localStorage.setItem('tpid', tpid);
    localStorage.setItem('tpname', tpname);
}

function loadTestPaperPro() {
    username = localStorage.getItem('username');
    tpid = localStorage.getItem('tpid');
    var arg = "{\"infotype\":\"LOADTEACHERTESTPAPERPRO\", \"username\": \"" + username + "\", \"testpapername\":\"" + tpid + "\"}";
    sendRequest(arg);
}

function load4teachertestpapercodepro(arg) {
    tcodeid = arg;
    localStorage.setItem('tcodeid', tcodeid);
}

function load4teachertestpapercodepros() {
    tpid = localStorage.getItem('tpid');
    tcodeid = localStorage.getItem('tcodeid');
    tcodenumber = localStorage.getItem('tcodenumber');
    var arg = "{\"infotype\":\"LOADTEACHERTESTPAPERCODEPRO\", \"tpid\": \"" + tpid + "\", \"tcodeid\":\"" + tcodeid + "\"}";
    sendRequest(arg);
}

function nextload4teachertestpapercodepro() {
    tcodeid = localStorage.getItem('tcodeid');
    tcodenumber = localStorage.getItem('tcodenumber');
    tpid = localStorage.getItem('tpid');
    if(tcodeid < tcodenumber - 1) {
        tcodeid++;
        localStorage.setItem('tcodeid', tcodeid);
        var arg = "{\"infotype\":\"LOADTEACHERTESTPAPERCODEPRO\", \"tpid\": \"" + tpid + "\", \"tcodeid\":\"" + tcodeid + "\"}";
        sendRequest(arg);
    }
    else {
        alert("没有下一题了~");
    }
}

function preload4teachertestpapercodepro() {
    tcodeid = localStorage.getItem('tcodeid');
    tcodenumber = localStorage.getItem('tcodenumber');
    if(tcodeid > 0) {
        tcodeid--;
        localStorage.setItem('tcodeid', tcodeid);
        var arg = "{\"infotype\":\"LOADTEACHERTESTPAPERCODEPRO\", \"tpid\": \"" + tpid + "\", \"tcodeid\":\"" + tcodeid + "\"}";
        sendRequest(arg);
    }
    else {
        alert("没有上一题了~");
    }
}

function submittestpaper() {
    tpid = localStorage.getItem('tpid');
    username = localStorage.getItem('username');
    let classid = document.getElementById("select4classid").value;
    let timelimit = document.getElementById("testpapertimelimit").value;
    if(timelimit == "" || timelimit < 0) {
        alert("限制时间不符合规范~");
        return;
    }
    timelimit *= 60;
    let arg = "{\"infotype\":\"SUBMITTESTPAPER\", \"username\": \"" + username + "\", \"tpid\":\"" + tpid + "\", \"classid\":\"" + classid + "\", \"timelimit\":\"" + timelimit + "\"}";
    sendRequest(arg);
}

function deletepaper() {
    let msg = "您真的确定要删除吗？\n\n请确认！";
	if (confirm(msg) == true) {
        tpid = localStorage.getItem('tpid');
        username = localStorage.getItem('username');
        let arg = "{\"infotype\":\"DELETETESTPAPER\", \"username\": \"" + username + "\", \"tpid\":\"" + tpid + "\"}";
        sendRequest(arg);
	}
}

function stuachievement() {
    tpid = localStorage.getItem('tpid');
    username = localStorage.getItem('username');
    let classid = document.getElementById("select4classid").value;
}

function load4achievement() {
    tpname = localStorage.getItem('tpname');
    let title4ac = document.getElementById("actitle");
    title4ac.innerHTML = "学生成绩：" + tpname;
    tpid = localStorage.getItem('tpid');
    let arg = "{\"infotype\":\"LOADACHIEVEMENTCLASS" + "\", \"tpid\":\"" + tpid + "\"}";
    sendRequest(arg);
}

function checkthescore() {
    tpid = localStorage.getItem('tpid');
    nowloadtestpaperselectnumber = localStorage.getItem('nowloadtestpaperselectnumber');
    nowloadtestpaperjudgenumber = localStorage.getItem('nowloadtestpaperjudgenumber');
    nowloadtestpapercodenumber = localStorage.getItem('nowloadtestpapercodenumber');
    let classids = document.getElementById("select4classidac").value;
    let arg = "{\"infotype\":\"LOADACHIEVEMENT" + "\", \"tpid\":\"" + tpid + "\", \"classid\":\"" + classids + "\", \"snumber\":\"" + nowloadtestpaperselectnumber + "\", \"jnumber\":\"" + nowloadtestpaperjudgenumber + "\", \"cnumber\":\"" + nowloadtestpapercodenumber + "\"}";
    sendRequest(arg);
}

function lookcode(arg) {
    arg = arg.replace(/~n/g, "\\n");
    arg = arg.replace(/~/g, "\n");
    arg = arg.replace(/`/g, "\"");
    alert(arg);
}
// --------------------------------------------------------------------------

// 学生考试 ------------------------------------------------------------------
function load4class() {
    username = localStorage.getItem('username');
    let arg = "{\"infotype\":\"LOADSTUCLASS\", \"username\": \"" + username + "\"}";
    sendRequest(arg);
}

function prompt4class(args) {
    let str = prompt(args);
    if(str) {
        let arg = "{\"infotype\":\"STUADDCLASS\", \"username\": \"" + username + "\", \"classid\":\"" + str + "\"}";
        sendRequest(arg);
    }
    else {
        prompt4class("班级号码有误，请重新输入！");
    }
}

function load4stutestpaper(tpid, tpname, starttime, timelimit, tusername) {
    localStorage.setItem('isend', false);
    localStorage.setItem('tpid', tpid);
    localStorage.setItem('tpname', tpname);
    localStorage.setItem('tusername', tusername);
    localStorage.setItem('starttime', starttime);
    localStorage.setItem('timelimit', timelimit);
}

function load4stutestpaperpro() {
    isend = localStorage.getItem('isend');
    tpid = localStorage.getItem('tpid');
    tpname = localStorage.getItem('tpname');
    tusername = localStorage.getItem('tusername');
    starttime = localStorage.getItem('starttime');
    timelimit = localStorage.getItem('timelimit');
    let headtppro = document.getElementById("headstutestpaperpro");
    headtppro.innerHTML = tpname;

    let timestamp = Date.parse(new Date());
    let times = parseInt(starttime) + parseInt(timelimit) * 1000;
    if(times < timestamp) {
        document.getElementById("submitstuexambtn").setAttribute("disabled", true);      //设置不可点击
        document.getElementById("submitstuexambtn").style.backgroundColor  = '#555555';  //设置背景色
        localStorage.setItem('isend', true);
        alert("本场考试已结束，将无法提交！");
    }

    let stutusername = document.getElementById("stutusername");
    stutusername.innerHTML = tusername;

    let stustarttime = document.getElementById("stustarttime");
    let date = new Date(parseInt(starttime));
    let YY = date.getFullYear() + '-';
    let MM = (date.getMonth() + 1 < 10 ? '0' + (date.getMonth() + 1) : date.getMonth() + 1) + '-';
    let DD = (date.getDate() < 10 ? '0' + (date.getDate()) : date.getDate());
    let hh = (date.getHours() < 10 ? '0' + date.getHours() : date.getHours()) + ':';
    let mm = (date.getMinutes() < 10 ? '0' + date.getMinutes() : date.getMinutes()) + ':';
    let ss = (date.getSeconds() < 10 ? '0' + date.getSeconds() : date.getSeconds());
    stustarttime.innerHTML = YY + MM + DD +" "+hh + mm + ss;
    
    let stutiimelimit = document.getElementById("stutiimelimit");
    stutiimelimit.innerHTML = timelimit / 60;

    let arg = "{\"infotype\":\"LOADSTUTESTPAPER\", \"username\": \"" + username + "\", \"testpapername\":\"" + tpid + "\"}";
    sendRequest(arg);
}

function submitstuexam() {
    selectpronumber = localStorage.getItem('selectpronumber');
    judgepronumber = localStorage.getItem('judgepronumber');
    codepronumber = localStorage.getItem('codepronumber');
    tpid = localStorage.getItem('tpid');
    username = localStorage.getItem('username');
    classid = localStorage.getItem('classid');
    tusername = localStorage.getItem('tusername');
    

    let arg = "{\"infotype\":\"SUBMITSTUTESTPAPERANSWER\", \"username\": \"" + username + "\", \"tpid\":\"" + tpid
              + "\", \"classid\":\"" + classid + "\", \"tusername\":\"" + tusername
              + "\", \"selectpronumber\":\"" + selectpronumber + "\", \"judgepronumber\":\"" + judgepronumber
              + "\", \"codepronumber\":\"" + codepronumber + "\"";
    for(let i = 0; i < selectpronumber; ++i) {
        let number = i + 1;
        let selectname = number + "_select";
        let selectanswer = number + "_select_answer";
        let radio = document.getElementsByName(selectname);
        let answer;
        for(let i = 0; i < radio.length; ++i) {
            if(radio[i].checked == true) {
                answer = radio[i].value;
                break;
            }
        }
        if(radio == undefined) {
            arg += ", \"" + selectanswer + "\": \"X\"";
        }
        else {
            arg += ", \"" + selectanswer + "\": \"" + answer + "\"";
        }
    }
    for(let i = 0; i < judgepronumber; ++i) {
        let number = i + 1;
        let judgename = number + "_judge";
        let judgeanswer = number + "_judge_answer";
        let radio = document.getElementsByName(judgename);
        let answer;
        for(let i = 0; i < radio.length; ++i) {
            if(radio[i].checked == true) {
                answer = radio[i].value;
                break;
            }
        }
        if(radio == undefined) {
            arg += ", \"" + judgeanswer + "\": \"X\"";
        }
        else {
            arg += ", \"" + judgeanswer + "\": \"" + answer + "\"";
        }
    }
    arg += "}";
    alert(arg);
    sendRequest(arg);
}

function submitcode4stucodepro() {
    tpid = localStorage.getItem('tpid');
    username = localStorage.getItem('username');
    classid = localStorage.getItem('classid');
    tusername = localStorage.getItem('tusername');
    tcodeid = localStorage.getItem('tcodeid');

    let answercode4exercise = document.getElementById("answercode4exercise").value;
    if(answercode4exercise == "") {
        alert("请在代码框输入你的代码~");
        return;
    }
    answercode4exercise = answercode4exercise.replace(/\\/g,"\\\\");
    answercode4exercise = answercode4exercise.replace(/"/g,"\\\"");
    alert(answercode4exercise);
    let type4exercise = "C";
    if(document.getElementById("type4exercise").value == 0) {
        type4exercise = "C";
    }
    else if(document.getElementById("type4exercise").value == 1) {
        type4exercise = "C++";
    }
    let fname = tpid + "_" + tcodeid;
    var arg = "{\"infotype\":\"SUBMITSTUEXAMCODE\", \"fname\":\"" + fname + "\", \"qname\":\"" + username + "\", \"username\":\"" + username + "\", \"tusername\":\"" + tusername + "\", \"classid\":\"" + classid + "\", \"type\":\"" + type4exercise + "\", \"code\":\"" + answercode4exercise + "\", \"id\":\"" + tpid + "\", \"number\":\"" + tcodeid + "\"}";
    sendRequest(arg);
}

function runcode4stucodepro() {
    tpid = localStorage.getItem('tpid');
    username = localStorage.getItem('username');
    classid = localStorage.getItem('classid');
    tusername = localStorage.getItem('tusername');
    tcodeid = localStorage.getItem('tcodeid');

    let answercode4exercise = document.getElementById("answercode4exercise").value;
    if(answercode4exercise == "") {
        alert("请在代码框输入你的代码~");
        return;
    }
    answercode4exercise = answercode4exercise.replace(/\\/g,"\\\\");
    answercode4exercise = answercode4exercise.replace(/"/g,"\\\"");
    alert(answercode4exercise);
    let type4exercise = "C";
    if(document.getElementById("type4exercise").value == 0) {
        type4exercise = "C";
    }
    else if(document.getElementById("type4exercise").value == 1) {
        type4exercise = "C++";
    }
    let fname = tpid + "_" + tcodeid;
    var arg = "{\"infotype\":\"RUNSTUEXAMCODE\", \"fname\":\"" + fname + "\", \"qname\":\"" + username + "\", \"username\":\"" + username + "\", \"tusername\":\"" + tusername + "\", \"classid\":\"" + classid + "\", \"type\":\"" + type4exercise + "\", \"code\":\"" + answercode4exercise + "\", \"id\":\"" + tpid + "\", \"number\":\"" + tcodeid + "\"}";
    sendRequest(arg);
}
// --------------------------------------------------------------------------

// 考生数据管理 --------------------------------------------------------------
function teachernewclass() {
    let tnewclassid = document.getElementById("newclassid").value;
    username = localStorage.getItem('username');
    var arg = "{\"infotype\":\"NEWCLASS\", \"tusername\":\"" + username + "\", \"classid\":\"" + tnewclassid + "\"}";
    sendRequest(arg);
}

function tload4Classes() {
    username = localStorage.getItem('username');
    var arg = "{\"infotype\":\"LOADTEACHERCLASS\", \"tusername\":\"" + username + "\"}";
    sendRequest(arg);
}

function tload4ClassesInfo(tusername, classid) {
    localStorage.setItem('nowloadclassid', classid);
}

function tload4ClassMember() {
    let classid = localStorage.getItem('nowloadclassid');
    let tusername = localStorage.getItem('username');
    var arg = "{\"infotype\":\"LOADCLASSINFO\", \"tusername\":\"" + tusername + "\", \"classid\":\"" + classid + "\"}";
    sendRequest(arg);
}

function tdelete4Classes(tusername, classid) {
    var msg = "您真的确定要删除吗？\n\n请确认！";
	if (confirm(msg) == true) {
        var arg = "{\"infotype\":\"DELETECLASS\", \"tusername\":\"" + tusername + "\", \"classid\":\"" + classid + "\"}";
        sendRequest(arg);
	}
}

function deleteclassmember(susername) {
	var msg = "您真的确定要删除吗？\n\n请确认！";
	if (confirm(msg) == true) {
        var arg = "{\"infotype\":\"DELETECLASSMEMBER\", \"username\":\"" + susername + "\"}";
        sendRequest(arg);
	}
}

function addclassmember() {
    let classid = localStorage.getItem('nowloadclassid');
    let tusername = localStorage.getItem('username');
    let newmembermaterial = document.getElementById("newmembermaterial").value;
    let newmembername = document.getElementById("newmembername").value;
    let newmemberusername = document.getElementById("newmemberusername").value;
    if(newmembermaterial == "" || newmembername == "" || newmemberusername == "") {
        alert("用户资料不完整，无法添加成员！");
    }
    else {
        var arg = "{\"infotype\":\"ADDCLASSMEMBER\", \"tusername\":\"" + tusername + "\", \"classid\":\"" + classid + "\", \"material\":\"" + newmembermaterial + "\", \"name\":\"" + newmembername + "\", \"username\":\"" + newmemberusername + "\"}";
        sendRequest(arg);
    }
}
// --------------------------------------------------------------------------

// 管理员操作 ----------------------------------------------------------------
function load4rootindex() {
    islogin = localStorage.getItem('islogin');
    if(islogin == "true") {
        var aObj = document.getElementById("username4root");
        username = localStorage.getItem('username');
        passwd = localStorage.getItem('password');
        usertype = localStorage.getItem('usertype');
        aObj.innerText = username;
    }
    else {
        alert("🐾请先登录！🐾");
        window.location.href = "../index.html";
    }
}

function rload4teacher() {
    var arg = "{\"infotype\":\"RLOADTEACHER\"}";
    sendRequest(arg);
}

function rdeleteteacher(username) {
    var msg = "您真的确定要删除吗？\n\n请确认！";
	if (confirm(msg) == true) {
        var arg = "{\"infotype\":\"RDELETETEACHER\", \"username\":\"" + username + "\"}";
        sendRequest(arg);
	}
}

function addrteacher() {
    let newmembermaterial = document.getElementById("newrmaterial").value;
    let newmembername = document.getElementById("newrname").value;
    let newmemberusername = document.getElementById("newrusername").value;
    if(newmembermaterial == "" || newmembername == "" || newmemberusername == "") {
        alert("用户资料不完整，无法添加成员！");
    }
    else {
        var arg = "{\"infotype\":\"ADDRTEACHER" + "\", \"material\":\"" + newmembermaterial + "\", \"name\":\"" + newmembername + "\", \"username\":\"" + newmemberusername + "\"}";
        sendRequest(arg);
    }
}

function executerootcode() {
    let rootcode = document.getElementById("rootcode").value;
    let rootoptions = document.getElementById("rootoptions").value;
    if(rootoptions == "flag") {
        var arg = "{\"infotype\":\"EXECUTEROOTCODE" + "\", \"option\":\"0\", \"code\":\"" + rootcode + "\"}";
        sendRequest(arg);
    }
    else {
        var arg = "{\"infotype\":\"EXECUTEROOTCODE" + "\", \"option\":\"1\", \"code\":\"" + rootcode + "\"}";
        sendRequest(arg);
    }
}
// --------------------------------------------------------------------------