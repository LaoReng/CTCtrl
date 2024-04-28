// 定义一个名为 SvgIcon 的全局组件
import { h } from 'vue'

const SvgIcon = {
    props: ['svgContent'],
    render() {
        return h('svg', {
            innerHTML: this.svgContent,
            // 添加其他必要的 SVG 属性，如 viewBox、width、height 等
        })
    },
}

export default SvgIcon;