import{I as k,r as i,f as r,g as c,i as t,h as e,w as s,t as h,X as M,Y as x,an as S,aI as I,aJ as L,aW as V,aX as q,C as D,D as C,e as z,H as E,j as w,a4 as U,a5 as A,T as R,u as X,b as B,o as H,q as O}from"./vendor-Dj-i8j__.js";import{_ as $}from"./index-qxZPit0M.js";const P=n=>(D("data-v-ade117d6"),n=n(),C(),n),j={class:"leftContainer"},J={class:"header"},W={class:"percentage-value"},Y=P(()=>t("span",{class:"percentage-label"},h("冷藏容量"),-1)),G={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},K={class:"food_list"},Q={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},Z={class:"food_list"},ee={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},te={class:"food_list"},oe=k({__name:"LeftContent",setup(n){const f=i("first"),N=[{color:"rgb(32,160,255)",percentage:50},{color:"#e6a23c",percentage:70},{color:"#f47920",percentage:90},{color:"#f56c6c",percentage:100}],y=[{Name:"苹果",Fd:"93%",Num:Math.floor(Math.random()*100)},{Name:"香蕉",Fd:"42%",Num:Math.floor(Math.random()*100)},{Name:"菠萝",Fd:"74%",Num:Math.floor(Math.random()*100)},{Name:"樱桃",Fd:"35%",Num:Math.floor(Math.random()*100)},{Name:"鸡蛋",Fd:"67%",Num:Math.floor(Math.random()*100)},{Name:"面包",Fd:"43%",Num:Math.floor(Math.random()*100)},{Name:"火龙果",Fd:"13%",Num:Math.floor(Math.random()*100)}],p=i([{foodType:"正常食物",color:"rgb(0, 255, 0)",per:53},{foodType:"即将过期食物",color:"rgb(255, 204, 0)",per:20},{foodType:"过期食物",color:"rgb(255,0, 0)",per:29}]),v=i([{Name:"苹果",Fd:"13%",Num:Math.floor(Math.random()*100)},{Name:"菠萝",Fd:"12%",Num:Math.floor(Math.random()*100)},{Name:"鸡蛋",Fd:"14%",Num:Math.floor(Math.random()*100)},{Name:"樱桃",Fd:"5%",Num:Math.floor(Math.random()*100)}]),b=i([{Name:"面包",Fd:"3%",Num:Math.floor(Math.random()*100)}]);return(F,g)=>{const u=S,o=I,l=L,d=V,_=q;return r(),c("div",j,[t("div",J,[e(u,{type:"dashboard",percentage:60,color:N},{default:s(({percentage:a})=>[t("span",W,h(a)+"%",1),Y]),_:1})]),e(_,{modelValue:f.value,"onUpdate:modelValue":g[0]||(g[0]=a=>f.value=a),class:"demo-tabs"},{default:s(()=>[e(d,{label:"食物情况",name:"first"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",G,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",K,[e(l,{data:y},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1})])])]),_:1}),e(d,{label:"即将过期",name:"second"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",Q,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",Z,[e(l,{data:v.value},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1},8,["data"])])])]),_:1}),e(d,{label:"到期列表",name:"third"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",ee,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",te,[e(l,{data:b.value},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1},8,["data"])])])]),_:1})]),_:1},8,["modelValue"])])}}}),ae=$(oe,[["__scopeId","data-v-ade117d6"]]),le=n=>(D("data-v-a9061ffe"),n=n(),C(),n),se={class:"leftContainer"},re={class:"header"},ne={class:"percentage-value"},de=le(()=>t("span",{class:"percentage-label"},h("冷藏容量"),-1)),ce={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},ie={class:"food_list"},pe={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},ue={class:"food_list"},_e={style:{"margin-bottom":"10px",color:"black","text-align":"left","font-size":"12px"}},me={class:"food_list"},fe=k({__name:"RightContent",setup(n){const f=i("first"),N=[{color:"rgb(32,160,255)",percentage:50},{color:"#e6a23c",percentage:70},{color:"#f47920",percentage:90},{color:"#f56c6c",percentage:100}],y=[{Name:"猪肉",Fd:"93%",Num:Math.floor(Math.random()*100)},{Name:"鸡肉",Fd:"42%",Num:Math.floor(Math.random()*100)},{Name:"冰淇淋",Fd:"74%",Num:Math.floor(Math.random()*100)},{Name:"火锅丸",Fd:"34%",Num:Math.floor(Math.random()*100)},{Name:"鱼",Fd:"72%",Num:Math.floor(Math.random()*100)},{Name:"帝王蟹",Fd:"25",Num:Math.floor(Math.random()*100)}],p=i([{foodType:"正常食物",color:"rgb(0, 255, 0)",per:70},{foodType:"即将过期食物",color:"rgb(255, 204, 0)",per:20},{foodType:"过期食物",color:"rgb(255,0, 0)",per:10}]),v=i([{Name:"鱼",Fd:"12%",Num:Math.floor(Math.random()*100)}]),b=i([{Name:"帝王蟹",Fd:"1",Num:Math.floor(Math.random()*100)}]);return(F,g)=>{const u=S,o=I,l=L,d=V,_=q;return r(),c("div",se,[t("div",re,[e(u,{type:"dashboard",percentage:83,color:N},{default:s(({percentage:a})=>[t("span",ne,h(a)+"%",1),de]),_:1})]),e(_,{modelValue:f.value,"onUpdate:modelValue":g[0]||(g[0]=a=>f.value=a),class:"demo-tabs"},{default:s(()=>[e(d,{label:"食物情况",name:"first"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",ce,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",ie,[e(l,{data:y},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1})])])]),_:1}),e(d,{label:"即将过期",name:"second"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",pe,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",ue,[e(l,{data:v.value},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1},8,["data"])])])]),_:1}),e(d,{label:"到期列表",name:"third"},{default:s(()=>[t("div",null,[(r(!0),c(M,null,x(p.value,(a,m)=>(r(),c("div",{style:{"margin-bottom":"10px"},key:m},[t("div",_e,h(a.foodType),1),e(u,{"text-inside":"",percentage:a.per,"stroke-width":16,striped:"","striped-flow":"",color:a.color},null,8,["percentage","color"])]))),128)),t("div",me,[e(l,{data:b.value},{default:s(()=>[e(o,{prop:"Name",label:"NAME"}),e(o,{prop:"Fd",label:"FD"}),e(o,{prop:"Num",label:"NUM"})]),_:1},8,["data"])])])]),_:1})]),_:1},8,["modelValue"])])}}}),he=$(fe,[["__scopeId","data-v-a9061ffe"]]),ve=k({__name:"ViewMain",setup(n){let f=i(!0);return(N,y)=>{const p=z("router-view");return r(),E(p,null,{default:s(({Component:v})=>[e(R,{name:"fade"},{default:s(()=>[w(f)?(r(),E(U(v),{key:0})):A("",!0)]),_:2},1024)]),_:1})}}}),be=$(ve,[["__scopeId","data-v-66f87362"]]),T=n=>(D("data-v-2a225b29"),n=n(),C(),n),ge={class:"container"},Ne={class:"door"},ye=T(()=>t("div",{class:"left_door"},[t("div",{class:"handle"},[t("div",{class:"left"}),t("div",{class:"right"})])],-1)),Me={class:"right_door"},xe=T(()=>t("div",{class:"handle"},[t("div",{class:"left"}),t("div",{class:"right"})],-1)),Fe={class:"view"},ke=T(()=>t("div",{style:{"margin-top":"5px"}},null,-1)),$e={class:"tabbar"},we={class:"content"},De={class:"leftDoorContent"},Ce={class:"rightDoorContent"},Te=k({__name:"index",setup(n){let f=X();const N=B({options:{fpsLimit:100,interactivity:{events:{onClick:{enable:!0,mode:"push"},onHover:{enable:!0,mode:"grab"},resize:!0},modes:{bubble:{distance:400,duration:2,opacity:.6,size:10},push:{quantity:4},repulse:{distance:200,duration:.4}}},particles:{color:{value:"#ffffff"},links:{color:"#ffffff",distance:150,enable:!0,opacity:.5,width:1},collisions:{enable:!0},move:{direction:"none",enable:!0,outMode:"bounce",random:!1,speed:2,straight:!1},number:{density:{enable:!0,value_area:800},value:60},opacity:{value:.5},shape:{type:"circle"},size:{random:!0,value:3}},detectRetina:!0}}),y=async o=>{await O(o)},p=async o=>{};let v=i(),b=i(!1),F=i(!1);const g=()=>{var d,_;let o=(d=document.querySelector(".left_door"))==null?void 0:d.querySelector(".handle"),l=(_=document.querySelector(".door"))==null?void 0:_.querySelector(".left_door");o==null||o.addEventListener("click",()=>{b.value=!b.value,b.value?l.style.transform="translateX(-100%)":l.style.transform="translateX(0%)"})},u=()=>{var d,_;let o=(d=document.querySelector(".right_door"))==null?void 0:d.querySelector(".handle"),l=(_=document.querySelector(".door"))==null?void 0:_.querySelector(".right_door");o==null||o.addEventListener("click",()=>{F.value=!F.value,F.value?l.style.transform="translateX(100%)":l.style.transform="translateX(0%)"})};return H(()=>{g(),u()}),(o,l)=>{const d=z("vue-particles");return r(),c("div",ge,[e(d,{id:"tsparticles",particlesInit:y,particlesLoaded:p,options:N.options},null,8,["options"]),t("div",Ne,[ye,t("div",Me,[xe,t("div",{class:"screen",ref_key:"screen",ref:v},[t("div",Fe,[ke,e(be)]),t("div",$e,[t("button",{class:"home",onClick:l[0]||(l[0]=_=>w(f).push("/embed/home"))}," 主页面 "),t("button",{class:"left",onClick:l[1]||(l[1]=_=>w(f).push("/embed/about"))}," 返回 ")])],512)]),t("div",we,[t("div",De,[e(ae)]),t("div",Ce,[e(he)])])])])}}}),Ie=$(Te,[["__scopeId","data-v-2a225b29"]]);export{Ie as default};