#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang='en'>

<head>
    <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
    <meta name='viewport' content='width=device-width' />

    <title> Ashtam's Lights </title>

    <script type='text/javascript' src='main.js'></script>
    <style>
        * {
            font-family: sans-serif;
            margin: 0;
            padding: 0;
        }
        
        body {
            /* color: #aaa; */
            background-color: #2F2F2F;
            text-align: center;
            overflow-x: hidden;
        }
        
        h1 {
            width: 65%;
            margin: 25px 0 25px 25%;
            color: #454545;
            text-align: center;
        }
        
        .header-wrap {
            color: #aaa;
            display: block;
            width: auto;
            height: auto;
            padding: 0;
            text-align: center;
            font-size: 45px;
            font-family: 'Press Start 2P', cursive;
            transform: perspective(200px) rotateX(10deg);
            letter-spacing: .1em;
            user-select: none;
            text-shadow: 0 -1px 0 #fff, 0 1px 0 #004d40, 0 2px 0 #00483B, 0 3px 0 #004639, 0 4px 0 #004336, 0 5px 0 #004134, 0 6px 0 #003F32, 0 7px 0 #003D30, 0 8px 0 #003A2D, 0 9px 0 #00382B, 0 10px 0 #003528, 0 11px 0 #003225, 0 12px 0 #002F22, 0 13px 0 #002B1E, 0 14px 0 #00281C, 0 15px 0 #001F13, 0 22px 30px rgba(0, 0, 0, 0.9), 0 22px 30px rgba(0, 0, 0, 0.9), 0 22px 15px rgba(0, 0, 0, 0.9), 0 11px 15px rgba(0, 0, 0, 0.9), 0 15px 20px rgba(0, 0, 0, 0.9), 0 15px 11px rgba(0, 0, 0, 0.9), 0 16px 11px rgba(0, 0, 0, 0.9);
            transition: text-shadow .3s ease .3s, transform .3s ease .3s, letter-spacing .3s ease .3s;
        }
        
        .header-wrap:hover {
            transition: text-shadow .33s ease, transform .3s ease, letter-spacing .3s ease;
            text-shadow: 0 0 0 #004134, 0 1px 0 #00483B, 0 2px 0 #003528, 0 3px 3px rgba(0, 0, 0, 0.9);
            transform: translate(0px, 15px) perspective(200px) rotateX(10deg);
            letter-spacing: .125em;
        }
        
        #colorbar {
            position: fixed;
            top: 20%;
            left: 20px;
            border: #454545 2px transparent;
        }
        
        #controls {
            width: 65%;
            float: right;
            margin-right: 10px;
            display: inline-block;
            padding-left: 5px;
        }
        
        ul {
            text-align: center;
        }
        
        ul#mode li {
            /* display:block; */
        }
        
        ul#brightness li,
        ul#speed li,
        ul#auto li {
            display: inline-block;
            width: 30%;
        }
        
        ul li a {
            display: block;
            margin: 3px;
            padding: 10px 5px;
            border: 2px solid #454545;
            border-radius: 5px;
            color: #454545;
            font-weight: bold;
            text-decoration: none;
        }
        
        ul li a.active {
            border: 2px solid #909090;
            color: #909090;
        }
        
        button {
            overflow: visible;
            border: 0;
            padding: 0;
            margin: 1.8rem;
        }
        
        .btn.striped-shadow span {
            display: block;
            position: relative;
            z-index: 2;
            height:75%;
            overflow-y:hidden;
            border: 5px solid;
            border-color: #4183D7;
            background: #77bfa1;
            color: #4183D7;
        }
        
        .btn.striped-shadow.white span {
            border-color: #fff;
            color: #fff;
            background: #77bfa1;
        }
        
        .btn.striped-shadow.blue span {
            border-color: #4183D7;
            background: #77bfa1;
            color: #4183D7;
        }
        
        .btn.striped-shadow.dark span {
            border-color: #393939;
            background: #77bfa1;
            color: #393939;
        }
        
        .btn {
            font-family: 'Bungee Shade', sans-serif;
            height: 80px;
            line-height: 65px;
            display: inline-block;
            letter-spacing: 1px;
            position: relative;
            font-size: 1.35rem;
            transition: opacity .3s, z-index .3s step-end, -webkit-transform .3s;
            transition: opacity .3s, z-index .3s step-end, transform .3s;
            transition: opacity .3s, z-index .3s step-end, transform .3s, -webkit-transform .3s;
            z-index: 1;
            background-color: transparent;
            cursor: pointer;
        }
        
        .btn {
            width: 35%;
            height: 48px;
            line-height: 38px;
            margin: 20px;
        }
        
        .btn.striped-shadow.white:after,
        .btn.striped-shadow.white:before {
            background-image: linear-gradient(135deg, transparent 0, transparent 5px, #fff 5px, #fff 10px, transparent 10px);
        }
        
        .btn.striped-shadow.blue:after,
        .btn.striped-shadow.blue:before {
            background-image: linear-gradient(135deg, transparent 0, transparent 5px, #4183D7 5px, #4183D7 10px, transparent 10px);
        }
        
        .btn.striped-shadow.dark:after,
        .btn.striped-shadow.dark:before {
            background-image: linear-gradient(135deg, transparent 0, transparent 5px, #393939 5px, #393939 10px, transparent 10px);
        }
        
        .btn.striped-shadow:hover:before {
            max-height: calc(100% - 10px);
        }
        
        .btn.striped-shadow:after {
            width: calc(100% - 4px);
            height: 8px;
            left: -10px;
            bottom: -9px;
            background-size: 15px 8px;
            background-repeat: repeat-x;
        }
        
        .btn.striped-shadow:after,
        .btn.striped-shadow:before {
            content: '';
            display: block;
            position: absolute;
            z-index: 1;
            transition: max-height .3s, width .3s, -webkit-transform .3s;
            transition: transform .3s, max-height .3s, width .3s;
            transition: transform .3s, max-height .3s, width .3s, -webkit-transform .3s;
            background-image: linear-gradient(135deg, transparent 0, transparent 5px, #4183D7 5px, #4183D7 10px, transparent 10px);
        }
        
        .btn.striped-shadow:hover {
            -webkit-transform: translate(-12px, 12px);
            -ms-transform: translate(-12px, 12px);
            transform: translate(-12px, 12px);
            z-index: 3;
        }
        
        .btn.striped-shadow:hover:after,
        .btn.striped-shadow:hover:before {
            -webkit-transform: translate(12px, -12px);
            -ms-transform: translate(12px, -12px);
            transform: translate(12px, -12px);
        }
        
        .btn.striped-shadow:before {
            width: 8px;
            max-height: calc(100% - 5px);
            height: 100%;
            left: -12px;
            bottom: -5px;
            background-size: 8px 15px;
            background-repeat: repeat-y;
            background-position: 0 100%;
        }
    </style>
</head>

<body>
    <h1 class="header-wrap"> लाइटे-ही-लाइटे</h1>
    <canvas id='colorbar' width='100' height='250'></canvas>
    <div id='controls'>
        <div id="mode">

        </div>

        <ul id='brightness'>
            <li><a href='#' class='b' id='-'>&#9788;</a></li>
            <li><a href='#' class='b' id='+'>&#9728;</a></li>
        </ul>

        <ul id='speed'>
            <li><a href='#' class='s' id='-'>&#8722;</a></li>
            <li><a href='#' class='s' id='+'>&#43;</a></li>
        </ul>

        <ul id='auto'>
            <li><a href='#' class='a' id='-'>&#9632;</a></li>
            <li><a href='#' class='a' id='+'>&#9658;</a></li>
        </ul>
    </div>
</body>

</html>
)=====";

