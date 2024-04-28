// 新建一个utils.js文件来放置这个工具函数

export function imgScaling(imgWidth, imgHeight, containerWidth, containerHeight) {
    let [
        // 用于设定图片的宽和高
        tempWidth,
        tempHeight,
    ] = [
            undefined,
            undefined
        ]
    try {
        imgWidth = parseFloat(imgWidth)
        imgHeight = parseFloat(imgHeight)
        containerWidth = parseFloat(containerWidth)
        containerHeight = parseFloat(containerHeight)
    } catch (error) {
        throw new Error('抱歉，我只接收数值类型或者可以转成数值类型的参数')
    }

    if (imgWidth > 0 && imgHeight > 0) {
        //原图片宽高比例 大于 指定的宽高比例，这就说明了原图片的宽度必然 > 高度
        if (imgWidth / imgHeight >= containerWidth / containerHeight) {
            if (imgWidth > containerWidth) {
                // alert('aaaaaaaa')
                tempWidth = containerWidth
                // 按原图片的比例进行缩放
                tempHeight = (imgHeight * containerWidth) / imgWidth
            } else {
                // 按照图片的大小进行缩放
                tempWidth = imgWidth
                tempHeight = imgHeight
            }
        } else {  // 原图片的高度必然 > 宽度
            if (imgHeight > containerHeight) {

                tempHeight = containerHeight
                // 按原图片的比例进行缩放
                tempWidth = (imgWidth * containerHeight) / imgHeight
            } else {
                // 按原图片的大小进行缩放
                tempWidth = imgWidth
                tempHeight = imgHeight
            }
        }
    }

    return { tempWidth, tempHeight }
}