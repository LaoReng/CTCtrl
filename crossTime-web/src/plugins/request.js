import axios from "axios"
import axiosRetry from 'axios-retry'
/**
 * 发生异常的情况
 * 自动再次发送请求
 */

axiosRetry(axios, {
    retries: 3, //请求失败重试次数
    retryDelay: (retryCount) => {
        return retryCount * 2000  //延迟请求  防止短时间内发送过于频繁
    },
    shouldResetTimeout: true
})

const config = {
    headers: {
        "Content-type": "application/json",
        "Accept": "application/json",
        'Cache-Control': 'no-cache',
        "cache": false,

    },
    //自动判断当前是开发环境还是生产环境，如果是开发环境，就走.env.development的逻辑，反之。。。生产环境
    baseURL: process.env.VUE_APP_URL
    // baseURL: "/lwd"
}


// const config = {
//     headers: {
//         "Content-type": "application/json",
//         "Accept": "application/json",
//         'Cache-Control': 'no-cache',
//         "cache": false,

//     },
//     baseURL: '/lwd',
//     withCredentials: false,

// }

const ajax = axios.create(config)

// const ajax = axios.create({
//     baseURL: "/lwd", //前缀
//     withCredentials: false,
//   });
// ajax.defaults.headers.options['Content-Type'] = 'application/json;charset=utf-8';



/**
 * 前后端基本都是 JWT鉴权
 * 所以我们需要在 发送请求的时候在 Header里携带 Token
 */
ajax.interceptors.request.use(config => {
    const token = localStorage.getItem("token")
    // console.log(process.env);
    // if (token) {
    config.headers["Authorization"] = token
    // console.log("AUtho", token);
    // }
    return config;
}, error => {
    return Promise.reject(error)
})

/**
 * 响应拦截器 返回页面数据
 * 把Ajax 其他的不相关的数据 过滤掉
 */
ajax.interceptors.response.use(response => {
    const data = response.data
    return data;
}, error => {
    return Promise.reject(error)
})

const post = (url, data) => ajax.post(url, data)
const put = (url, data) => ajax.put(url, data)
const get = (url, data) => ajax.get(url, { params: data })
const deleted = (url, data) => ajax.request({
    url: url,
    method: "delete",
    data: data
})

export { post, get, put, deleted, ajax }
