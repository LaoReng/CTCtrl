import{a as r}from"./vendor-Dj-i8j__.js";import{G as s}from"./index-2bWTvhk3.js";const o={baseURL:"http://www.ctctrl.nat300.top/api",timeout:5e3},e=r.create(o);e.interceptors.request.use(t=>(t.headers.Authorization=s(),t));e.interceptors.response.use(t=>t.data);export{e as r};