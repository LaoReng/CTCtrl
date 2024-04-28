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
        "Accept": "application/json"
    },
    baseURL: process.env.VUE_APP_URL
}

const ajax = axios.create(config)

/**
 * 前后端基本都是 JWT鉴权
 * 所以我们需要在 发送请求的时候在 Header里携带 Token
 */
ajax.interceptors.request.use(config => {
    const token = sessionStorage.getItem("token")
    if (token) {
        config.headers["Authorization"] = token
    }
    return config
}, error => {
    return Promise.reject(error)
})

/**
 * 响应拦截器 返回页面数据
 * 把Ajax 其他的不相关的数据 过滤掉
 */
ajax.interceptors.response.use(response => {
    // const data = response.data
    return response.data;
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

export { post, get, put, deleted }
