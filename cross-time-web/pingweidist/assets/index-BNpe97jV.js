import{I as k,r as m,o as q,f as B,g as T,i as d,h as e,w as a,n as s,t as M,j as f,p as g,E as N,aI as P,aK as $,y as z,aL as H,aJ as U,aM as j,x as J,ae as K,C as L,D as R}from"./vendor-Dj-i8j__.js";import{H as A}from"./home-BQ6SLRyk.js";import{_ as F}from"./index-2bWTvhk3.js";const G=i=>(L("data-v-19b20447"),i=i(),R(),i),O={class:"box"},Q={class:"elm-box"},W={class:"page-box"},X=G(()=>d("div",{style:{"margin-bottom":"1rem"}},"请输入要更改的设备名：",-1)),Y={class:"dialog-footer"},Z=k({__name:"index",setup(i){const p=A();let u=m([]),o=m(!1),n=m(""),b=-1;const v=(_,l)=>{o.value=!0,b=l},x=()=>{p.equipInfo.equipments[b].name=n.value,o.value=!1},y=_=>{u.value.splice(_,1),N({type:"success",message:"删除成功"})},C=()=>{};return q(async()=>{await p.init(),p.equipInfo.equipments&&(u.value=[...p.equipInfo.equipments])}),(_,l)=>{const c=P,I=$,r=z,E=H,V=U,S=j,h=J,w=K;return B(),T("div",O,[d("div",Q,[e(V,{align:"center",data:f(u)},{default:a(()=>[e(c,{align:"center",prop:"name",label:"设备名"}),e(c,{align:"center",prop:"ip",label:"设备IP"}),e(c,{prop:"eStatus",label:"设备状况"},{default:a(t=>[e(I,{type:t.row.eStatus?"success":"danger"},{default:a(()=>[s(M(t.row.eStatus?"正常":"异常"),1)]),_:2},1032,["type"])]),_:1}),e(c,{align:"center",fixed:"right",label:"操作"},{default:a(t=>[e(r,{type:"primary",size:"small",onClick:D=>v(t.row,t.$index)},{default:a(()=>[s(" 更改 ")]),_:2},1032,["onClick"]),e(E,{onCancel:C,onConfirm:D=>y(t.$index),title:"确定删除吗？"},{reference:a(()=>[e(r,{type:"danger",size:"small"},{default:a(()=>[s("删除")]),_:1})]),_:2},1032,["onConfirm"])]),_:1})]),_:1},8,["data"])]),d("div",W,[e(S,{small:"",background:"",layout:"prev, pager, next",total:1,class:"mt-4"})]),e(w,{modelValue:f(o),"onUpdate:modelValue":l[2]||(l[2]=t=>g(o)?o.value=t:o=t),width:"500"},{footer:a(()=>[d("div",Y,[e(r,{onClick:l[1]||(l[1]=t=>g(o)?o.value=!1:o=!1)},{default:a(()=>[s("取消")]),_:1}),e(r,{type:"primary",onClick:x},{default:a(()=>[s("确定")]),_:1})])]),default:a(()=>[X,e(h,{modelValue:f(n),"onUpdate:modelValue":l[0]||(l[0]=t=>g(n)?n.value=t:n=t),placeholder:"请输入要更改的设备名："},null,8,["modelValue"])]),_:1},8,["modelValue"])])}}}),oe=F(Z,[["__scopeId","data-v-19b20447"]]);export{oe as default};