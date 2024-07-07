import{I as m,r as f,o as U,f as c,H as x,w as $,g as w,Y as D,J as L,i as s,t as B,j as d,X as H,aY as q,h as n,ao as z,C as A,D as X,e as k,l as h,v,p as N,aZ as Y,a_ as j,a$ as F}from"./vendor-Dj-i8j__.js";import{_ as V,s as J}from"./index-qxZPit0M.js";const P=["onClick"],Z=m({__name:"index",emits:["child-click"],setup(a,{emit:t}){const o=t;let e=f([{tagName:"注册",isRed:!0,id:1},{tagName:"登录",isRed:!1,id:2},{tagName:"下载客户端",isRed:!1,id:3},{tagName:"安装客户端",isRed:!1,id:4},{tagName:"如何控制设备",isRed:!1,id:5}]);const i=(l,g)=>{for(let p=0;p<e.value.length;p++)e.value[p].isRed=!1;e.value[g].isRed=!0,o("child-click",l)};return U(()=>{o("child-click",e.value[0])}),(l,g)=>{const p=q;return c(),x(p,{width:"300px"},{default:$(()=>[(c(!0),w(H,null,D(d(e),(u,r)=>(c(),w("div",{style:L({background:u.isRed?"red":"",color:u.isRed?"white":""}),key:u.id,onClick:_=>i(u,r)},[s("span",null,B(u.tagName),1)],12,P))),128))]),_:1})}}}),G=V(Z,[["__scopeId","data-v-283da464"]]),K="/assets/logo-DCqXVln2.png",b=a=>(A("data-v-bfb3c6dc"),a=a(),X(),a),O={class:"container"},Q={class:"left"},W={class:"title"},ee=b(()=>s("div",{class:"date"},"更新时间:2024-06-25 23:47:13",-1)),te={class:"right"},le={class:"img"},oe=b(()=>s("img",{src:K},null,-1)),ie=b(()=>s("div",{class:"name"},"CTCtrl开发团队",-1)),ne=m({__name:"index",setup(a){return(t,o)=>{const e=z;return c(),w("div",O,[s("div",Q,[s("div",W,B(d(J).title)+"使用说明文档",1),ee]),s("div",te,[s("div",le,[n(e,{size:100},{default:$(()=>[oe]),_:1})]),ie])])}}}),se=V(ne,[["__scopeId","data-v-bfb3c6dc"]]),ae=m({__name:"process1",setup(a){const t=`
<h1 align="center">注册</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol> 
  <li>用户首先输入昵称,手机号和密码注册账号</li>
  <img src="http://118.190.96.96:90/image//图片1.png"}">
  <li>点击确定后会跳转到问题设置页面,用户需要设置两个问题用于忘记密码输入答案来找回密码,设置完问题后点击注册即可完成注册</li>
<img src="http://118.190.96.96:90/image//图片2.png"}">
  </ol>
`;return(o,e)=>{const i=k("v-md-editor");return c(),x(i,{mode:"preview",modelValue:t,"onUpdate:modelValue":e[0]||(e[0]=l=>t=l),height:"100%"})}}}),ce=m({__name:"process2",setup(a){const t=`
<h1 align="center">登录</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>输入手机号/用户名和密码进行登录,如果忘记密码,可以点击忘记密码跳转到获取问题页面</li>
  <img src="http://118.190.96.96:90/image//图片3.png"}">
  <li>输入手机号/用户名,点击获取问题按钮跳转到问题验证页面</li>
<img src="http://118.190.96.96:90/image//图片4.png"}">
  <li>输入两个问题的答案,点击提交即可跳转到用户登录页面,并且密码会自动填充</li>
<img src="http://118.190.96.96:90/image//图片5.png"}">
  </ol>
`;return(o,e)=>{const i=k("v-md-editor");return c(),x(i,{mode:"preview",modelValue:t,"onUpdate:modelValue":e[0]||(e[0]=l=>t=l),"left-toolbar":"undo redo | tip",height:"100%"})}}}),re=m({__name:"process3",setup(a){const t=`
<h1 align="center">下载客户端</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>可以通过登录页面右下角的下载中心或者平台菜单栏的客户端下载按钮进入下载中心</li>
  <img src="http://118.190.96.96:90/image//图片6.png"}">
  <img src="http://118.190.96.96:90/image//图片7.png"}">
  <li>点击下载即可下载客户端</li>
<img src="http://118.190.96.96:90/image//图片8.png"}">
  </ol>
`;return(o,e)=>{const i=k("v-md-editor");return c(),x(i,{mode:"preview",modelValue:t,"onUpdate:modelValue":e[0]||(e[0]=l=>t=l),"left-toolbar":"undo redo | tip",height:"100%"})}}}),de=m({__name:"process4",setup(a){const t=`
<h1 align="center">安装客户端</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>双击下载好的客户端程序，进行被控端程序安装</li>
<img src="http://118.190.96.96:90/image//图片9.png"}">
  <li>选择程序安装类型,点击下一步</li>
<img src="http://118.190.96.96:90/image//图片10.png"}">
  <li>查看程序安装说明文件,点击下一步</li>
<img src="http://118.190.96.96:90/image//图片11.png"}">
  <li>选择程序安装位置并同意协议约定条目,点击下一步</li>
<img src="http://118.190.96.96:90/image//图片12.png"}">
  <li>按需求选择要安装的功能并点击下一步</li>
<img src="http://118.190.96.96:90/image//图片13.png"}">
  <li>点击安装等到进度条结束完成安装</li>
<img src="http://118.190.96.96:90/image//图片14.png"}">
  </ol>
`;return(o,e)=>{const i=k("v-md-editor");return c(),x(i,{mode:"preview",modelValue:t,"onUpdate:modelValue":e[0]||(e[0]=l=>t=l),"left-toolbar":"undo redo | tip",height:"100%"})}}}),_e=m({__name:"process5",setup(a){const t=`
<h1 align="center">如何控制设备</h1>
<div align="center" style="color:#ccc">最后更新时间: 2024-06-25 23:47:13</div>
<hr/>
<ol>
  <li>安装完客户端后,用户在客户端进行登录,登录后此设备就会自动归属于该用户的设备</li>
  <img src="http://118.190.96.96:90/image//图片15.png"}">
  <li>被控者可以在被控端的软件托盘右键点击查看控制码获取连接密钥</li>
<img src="http://118.190.96.96:90/image//图片16.png"}">
  <li>双击或者右键点击连接就会弹出密钥框</li>
<img src="http://118.190.96.96:90/image//图片17.png"}">
  <li>输入正确的密钥,点击确定</li>
<img src="http://118.190.96.96:90/image//图片18.png"}">
  <li>验证成功后即可进入该设备的监控页面</li>
<img src="http://118.190.96.96:90/image//图片19.png"}">
  </ol>
`;return(o,e)=>{const i=k("v-md-editor");return c(),x(i,{mode:"preview",modelValue:t,"onUpdate:modelValue":e[0]||(e[0]=l=>t=l),"left-toolbar":"undo redo | tip",height:"100%"})}}}),me={t:"1719023197877",class:"icon",viewBox:"0 0 1024 1024",version:"1.1",xmlns:"http://www.w3.org/2000/svg","p-id":"4540",width:"16",height:"16"},pe=["fill"],ue={class:"desc-container"},ge=m({__name:"index",setup(a){let t=f(0),o=f(!1),e=f(!1);const i=f(null);let l=f(),g=document.querySelector("body");g==null||g.addEventListener("scroll",r=>{var C;document.documentElement.clientHeight<((C=r.target)==null?void 0:C.scrollTop)?o.value=!0:o.value=!1});const p=()=>{t.value=document.documentElement.scrollTop||document.body.scrollTop;let r=t.value/200,_=setInterval(()=>{t.value-=r,r+=20,t.value<=0&&(clearInterval(_),o.value=!1),document.body.scrollTop=t.value,document.documentElement.scrollTop=t.value},30)},u=r=>{l.value=r};return(r,_)=>{const C=Y,M=j,I=F;return c(),w(H,null,[h(s("div",{class:"toTop",onMouseenter:_[0]||(_[0]=y=>N(e)?e.value=!0:e=!0),onMouseleave:_[1]||(_[1]=y=>N(e)?e.value=!1:e=!1),onClick:p},[(c(),w("svg",me,[s("path",{d:"M946.494 754.216c19.445-19.445 19.445-51.265 0-70.711L547.355 284.368c-19.445-19.445-51.265-19.445-70.711 0L77.506 683.506c-19.445 19.445-19.445 51.265 0 70.711","p-id":"4541",fill:d(e)?"#ffff":"#337ab7"},null,8,pe)]))],544),[[v,d(o)]]),s("div",ue,[s("div",{class:"common-layout",ref_key:"descContainer",ref:i},[n(I,null,{default:$(()=>[n(C,null,{default:$(()=>[n(se)]),_:1}),n(I,null,{default:$(()=>[n(G,{onChildClick:u}),n(M,null,{default:$(()=>{var y,S,T,E,R;return[h(n(ae,null,null,512),[[v,((y=d(l))==null?void 0:y.id)==1]]),h(n(ce,null,null,512),[[v,((S=d(l))==null?void 0:S.id)==2]]),h(n(re,null,null,512),[[v,((T=d(l))==null?void 0:T.id)==3]]),h(n(de,null,null,512),[[v,((E=d(l))==null?void 0:E.id)==4]]),h(n(_e,null,null,512),[[v,((R=d(l))==null?void 0:R.id)==5]])]}),_:1})]),_:1})]),_:1})],512)])],64)}}}),fe=V(ge,[["__scopeId","data-v-44319332"]]);export{fe as default};
