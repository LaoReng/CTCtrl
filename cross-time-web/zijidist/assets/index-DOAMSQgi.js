import{r as v,u as z,b as f,e as B,f as c,g as m,h as e,w as r,T as N,q as A,i as y,t as $,j as d,k as j,$ as D,m as k,a0 as w,n as x,E as g,s as M,x as O,y as H,z as K,A as G}from"./vendor-Dj-i8j__.js";import{_ as J,u as Q,s as W,S as X,a as Y}from"./index-qxZPit0M.js";const Z={class:"loginContainer"},ee={class:"login"},oe={class:"header"},se={key:0},ae={key:1},te={key:2,style:{"text-align":"center"}},re={key:3,style:{"text-align":"center"}},ne={style:{"text-align":"center","margin-top":"1rem"}},le={__name:"index",setup(ie){const V=Q(),q=v(),b=z(),C=f({options:{fpsLimit:100,interactivity:{events:{onClick:{enable:!0,mode:"push"},onHover:{enable:!0,mode:"grab"},resize:!0},modes:{bubble:{distance:400,duration:2,opacity:.6,size:10},push:{quantity:4},repulse:{distance:200,duration:.4}}},particles:{color:{value:"#ffffff"},links:{color:"#ffffff",distance:150,enable:!0,opacity:.5,width:1},collisions:{enable:!0},move:{direction:"none",enable:!0,outMode:"bounce",random:!1,speed:2,straight:!1},number:{density:{enable:!0,value_area:800},value:60},opacity:{value:.5},shape:{type:"circle"},size:{random:!0,value:3}},detectRetina:!0}}),U=async i=>{await A(i)},F=async i=>{},t=f({username:"",phone:"",password:"",repassword:""}),s=f({question1:"",answer1:"",question2:"",answer2:""}),_=v(!1),l=f({phone:"",username:"",password:"",balance:0,question1:"",answer1:"",question2:"",answer2:"",registerTime:new Date().toLocaleString().replaceAll("/","-")}),L={username:[{required:!0,message:"必须输入用户昵称",trigger:"change"}],phone:[{required:!0,validator:(i,o,u)=>{let n=/^(13[0-9]|14[01456879]|15[0-35-9]|16[2567]|17[0-8]|18[0-9]|19[0-35-9])\d{8}$/;o==""?u(new Error("请输入你的手机号")):n.test(o)?u():u(new Error("请正确填写您的手机号码！"))},trigger:"change"}],password:[{required:!0,message:"必须输入密码！",trigger:"change"},{min:6,max:16,message:"密码长度在6-16位之间！",trigger:"change"}],repassword:[{required:!0,validator:(i,o,u)=>{o===""?u(new Error("请再次输入密码")):o!=t.password?u(new Error("两次输入密码不一致!")):u()},trigger:"change"},{min:6,max:16,message:"密码长度在6-16位之间",trigger:"change"}]},h=v(!1),R=()=>{q.value.validate(async i=>{i&&(_.value=!0,g({type:"info",message:"为了保证数据安全，请填写找回密码的相关问题",duration:"1000",showClose:!0}))})},S=async()=>{if(s.question1.trim()==""||s.question2.trim()==""||s.answer1.trim()==""||s.answer2.trim()=="")g({type:"error",message:"请正确填写问题或者答案",duration:"2000",showClose:!0});else{l.phone=t.phone,l.password=t.password,l.username=t.username,l.question1=s.question1,l.question2=s.question2,l.answer1=s.answer1,l.answer2=s.answer2,h.value=!0;let i=await V.userRegister(l);h.value=!1,i.status==0?(await V.userLogin({phone:l.phone,password:l.password}),g({type:"success",message:"注册成功",duration:"2000",showClose:!0}),X(t.phone),await Y({avatar:"/1.jpg"}),setTimeout(()=>{b.push("/home")},1500)):g({type:"error",message:i.message,duration:"2000",showClose:!0})}},T=async()=>{b.push("/newlogin")};return(i,o)=>{const u=B("vue-particles"),n=M,p=O,E=H,I=K,P=G;return c(),m("div",Z,[e(u,{id:"tsparticles",particlesInit:U,particlesLoaded:F,options:C.options},null,8,["options"]),e(N,{name:"fade-down",appear:""},{default:r(()=>[y("div",ee,[e(P,{ref_key:"ruleFormRef",ref:q,model:t,rules:L,"status-icon":""},{default:r(()=>[e(n,null,{default:r(()=>[y("div",oe,$(d(W).title)+"注册页面",1)]),_:1}),_.value?(c(),m("div",ae,[e(n,{prop:"question1"},{default:r(()=>[e(p,{"prefix-icon":d(w),modelValue:s.question1,"onUpdate:modelValue":o[4]||(o[4]=a=>s.question1=a),placeholder:"问题1"},null,8,["prefix-icon","modelValue"])]),_:1}),e(n,{prop:"answer1"},{default:r(()=>[e(p,{modelValue:s.answer1,"onUpdate:modelValue":o[5]||(o[5]=a=>s.answer1=a),placeholder:"答案1","prefix-icon":d(w)},null,8,["modelValue","prefix-icon"])]),_:1}),e(n,{prop:"question2"},{default:r(()=>[e(p,{modelValue:s.question2,"onUpdate:modelValue":o[6]||(o[6]=a=>s.question2=a),placeholder:"问题2","prefix-icon":d(w)},null,8,["modelValue","prefix-icon"])]),_:1}),e(n,{prop:"answer2"},{default:r(()=>[e(p,{modelValue:s.answer2,"onUpdate:modelValue":o[7]||(o[7]=a=>s.answer2=a),"prefix-icon":d(w),placeholder:"答案2"},null,8,["modelValue","prefix-icon"])]),_:1})])):(c(),m("div",se,[e(n,{prop:"username"},{default:r(()=>[e(p,{"prefix-icon":d(j),modelValue:t.username,"onUpdate:modelValue":o[0]||(o[0]=a=>t.username=a),placeholder:"用户昵称"},null,8,["prefix-icon","modelValue"])]),_:1}),e(n,{prop:"phone"},{default:r(()=>[e(p,{"prefix-icon":d(D),modelValue:t.phone,"onUpdate:modelValue":o[1]||(o[1]=a=>t.phone=a),placeholder:"手机号"},null,8,["prefix-icon","modelValue"])]),_:1}),e(n,{prop:"password"},{default:r(()=>[e(p,{modelValue:t.password,"onUpdate:modelValue":o[2]||(o[2]=a=>t.password=a),"prefix-icon":d(k),"show-password":"",placeholder:"密码"},null,8,["modelValue","prefix-icon"])]),_:1}),e(n,{prop:"repassword"},{default:r(()=>[e(p,{modelValue:t.repassword,"onUpdate:modelValue":o[3]||(o[3]=a=>t.repassword=a),"prefix-icon":d(k),"show-password":"",placeholder:"重复密码"},null,8,["modelValue","prefix-icon"])]),_:1})])),_.value?(c(),m("div",re,[e(E,{style:{width:"50%","border-radius":"10px"},type:"primary",color:"#626aef",class:"btn",loading:h.value,onClick:S},{default:r(()=>[x(" 注册 ")]),_:1},8,["loading"])])):(c(),m("div",te,[e(E,{style:{width:"50%","border-radius":"10px"},type:"primary",color:"#626aef",class:"btn",onClick:R},{default:r(()=>[x(" 确定 ")]),_:1})])),y("div",ne,[e(I,{type:"warning",onClick:T},{default:r(()=>[x("已有账号去登录？")]),_:1})])]),_:1},8,["model"])])]),_:1})])}}},me=J(le,[["__scopeId","data-v-7ab2bc10"]]);export{me as default};