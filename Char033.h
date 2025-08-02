#ifndef Char033_H
#define Char033_H
#include <iostream>

namespace c033{

/**
 * @brief 基类，定义终端控制字符的基本接口
 * @details 所有终端控制字符类必须继承此类并实现 operator std::string() 方法
 */
class Char033{
protected:
	using uint=unsigned int;
	constexpr Char033(){}
	~Char033()=default;
public:
	virtual operator std::string()const=0;
};

#ifdef _WIN32
namespace{
class Char033Init{
	Char033Init(){
		system("cls");
	}
}initer;
}
#endif

inline std::ostream& operator<<(std::ostream& out,const Char033& c){
	return out<<std::string(c);
}

/**
 * @brief 控制终端光标的移动
 * @details 通过 x 和 y 坐标控制光标的上下左右移动
 * @param x 垂直方向移动距离（正数向下，负数向上）
 * @param y 水平方向移动距离（正数向右，负数向左）
 */
class CursorMove: public Char033{
public:
	int x=0,y=0;
	CursorMove(){}
	CursorMove(int xx,int yy):x(xx),y(yy){}
	inline operator std::string()const override{
		std::string ret;
		if(x>0){
			ret+="\033["+std::to_string(x)+"B";
		}
		if(x<0){
			ret+="\033["+std::to_string(-x)+"A";
		}
		if(y>0){
			ret+="\033["+std::to_string(y)+"C";
		}
		if(y<0){
			ret+="\033["+std::to_string(-y)+"D";
		}
		return ret;
	}
};
/**
 * @brief 设置终端光标的绝对位置
 * @details 将光标移动到指定的 (x, y) 坐标位置
 * @param x 垂直方向坐标（从 0 开始）
 * @param y 水平方向坐标（从 0 开始）
 */
class CursorSet: public Char033{
public:
	uint x,y;
	CursorSet(uint xx,uint yy):x(xx),y(yy){}
	inline operator std::string()const override{
		return "\033["+std::to_string(x+1)+";"+std::to_string(y+1)+"H";
	}
};
/**
 * @brief 控制终端屏幕的滚动
 * @details 通过 line 参数控制屏幕向上或向下滚动
 * @param line 滚动行数（正数向下滚动，负数向上滚动）
 */
class ScreenScroll: public Char033{
public:
	int line=0;
	ScreenScroll(int l=0):line(l){}
	inline operator std::string()const override{
		if(line>0){
			return "\033["+std::to_string(line)+"S";
		}
		if(line<0){
			return "\033["+std::to_string(-line)+"T";
		}
		return "";
	}
};
/**
 * @brief 保存当前光标位置
 * @details 保存光标位置以便后续恢复
 */
class CursorSave: public Char033{
public:
	inline operator std::string()const override{
		return "\033[s";
	}
};
/**
 * @brief 恢复之前保存的光标位置
 * @details 将光标恢复到之前保存的位置
 */
class CursorLoad: public Char033{
public:
	inline operator std::string()const override{
		return "\033[u";
	}
};
/**
 * @brief 隐藏终端光标
 * @details 隐藏光标以提升界面美观性
 */
class CursorHide: public Char033{
public:
	inline operator std::string()const override{
		return "\033[?25l";
	}
};
/**
 * @brief 显示终端光标
 * @details 恢复光标的显示状态
 */
class CursorShow: public Char033{
public:
	inline operator std::string()const override{
		return "\033[?25h";
	}
};
/**
 * @brief 清除光标位置到屏幕末尾的内容
 * @details 从光标位置开始向后清除屏幕内容
 */
class ClearBackward: public Char033{
public:
	inline operator std::string()const override{
		return "\033[J";
	}
};
/**
 * @brief 清除光标位置到屏幕开头的内容
 * @details 从光标位置开始向前清除屏幕内容
 */
class ClearForward: public Char033{
public:
	inline operator std::string()const override{
		return "\033[1J";
	}
};
/**
 * @brief 清除整个屏幕内容
 * @details 清除终端屏幕上的所有内容
 */
class ClearAll: public Char033{
public:
	inline operator std::string()const override{
		return "\033[2J";
	}
};
/**
 * @brief 清除光标位置到行末尾的内容
 * @details 从光标位置开始向后清除当前行内容
 */
class ClearLineBackward: public Char033{
public:
	inline operator std::string()const override{
		return "\033[K";
	}
};
/**
 * @brief 清除光标位置到行开头的内容
 * @details 从光标位置开始向前清除当前行内容
 */
class ClearLineForward: public Char033{
public:
	inline operator std::string()const override{
		return "\033[1K";
	}
};
/**
 * @brief 清除整行内容
 * @details 清除当前行的所有内容
 */
class ClearLine: public Char033{
public:
	inline operator std::string()const override{
		return "\033[2K";
	}
};

/**
 * @brief 设置终端窗口标题
 * @param title 标题内容
 */
class TerminalTitle: public Char033{
public:
	std::string title;
	TerminalTitle(const std::string& t):title(t){}
	inline operator std::string()const override{
		return "\033]0;"+title+"\007";
	}
};

/**
 * @brief 调整终端窗口大小
 * @param rows 行数
 * @param cols 列数
 */
class TerminalSize: public Char033{
public:
	uint rows,cols;
	TerminalSize(uint r,uint c):rows(r),cols(c){}
	inline operator std::string()const override{
		return "\033[8;"+std::to_string(rows)+";"+std::to_string(cols)+"t";
	}
};

/**
 * @brief 在光标位置插入空格
 * @param count 空格数量
 */
class InsertSpace: public Char033{
public:
	uint count;
	InsertSpace(uint c):count(c){}
	inline operator std::string()const override{
		return "\033["+std::to_string(count)+"@";
	}
};

/**
 * @brief 在光标位置插入空行
 * @param count 行数
 */
class InsertLine: public Char033{
public:
	uint count;
	InsertLine(uint c):count(c){}
	inline operator std::string()const override{
		return "\033["+std::to_string(count)+"L";
	}
};

/**
 * @brief 触发终端铃声
 */
class Bell: public Char033{
public:
	inline operator std::string()const override{
		return "\007";
	}
};

/**
 * @brief 删除光标处的若干字符，后续字符前移
 * @param count 删除的字符数量
 */
class DeleteText: public Char033{
public:
	uint count;
	DeleteText(uint c):count(c){}
	inline operator std::string()const override{
		return "\033["+std::to_string(count)+"P";
	}
};

/**
 * @brief 删除光标处的若干行，后续行前移
 * @param count 删除的行数
 */
class DeleteLine: public Char033{
public:
	uint count;
	DeleteLine(uint c):count(c){}
	inline operator std::string()const override{
		return "\033["+std::to_string(count)+"M";
	}
};

/**
 * @brief 重置终端状态
 * @details 完全重置终端状态（包括光标位置、样式等）
 */
class Reset: public Char033{
public:
	inline operator std::string()const override{
		return "\033c";
	}
};

/**
 * @brief 自定义光标形状
 * @details 支持块状、下划线或竖线光标
 * @param shape 光标形状（Block=块状, Underline=下划线, Bar=竖线）
 */
class CursorShape : public Char033 {
public:
    enum Shape { Block = 1, Underline = 3, Bar = 5 };
    Shape shape;
    CursorShape(Shape s) : shape(s) {}
    inline operator std::string() const override {
        return "\033[" + std::to_string(static_cast<int>(shape)) + " q";
    }
};

/**
 * @brief 进入或退出备用屏幕缓冲区
 * @details 备用屏幕缓冲区用于临时保存当前屏幕内容，退出时恢复
 * @param enable 是否启用备用屏幕缓冲区
 */
class SwitchScreen: public Char033{
public:
	bool enable;
	SwitchScreen(bool e):enable(e){}
	inline operator std::string()const override{
		return enable ? "\033[?1049h" : "\033[?1049l";
	}
};

/**
 * @brief 控制终端文本样式
 * @details 支持多种文本样式，包括颜色、背景色、下划线、闪烁等
 * @param underline 下划线样式（无/单线/双线）
 * @param brightness 文本亮度（隐藏/暗/正常/亮）
 * @param italic 是否斜体
 * @param deleted 是否删除线
 * @param flicker 是否闪烁
 * @param reversed 是否反色
 * @param color 文本颜色（支持16种预定义颜色和RGB自定义颜色）
 * @param bkcolor 背景颜色（支持16种预定义颜色和RGB自定义颜色）
 * @param r,g,b 自定义文本颜色的RGB值
 * @param bkr,bkg,bkb 自定义背景颜色的RGB值
 */
class Pattern: public Char033{
public:
	/**
	 * @brief 下划线样式枚举
	 * @param lNone 无下划线
	 * @param lOne 单下划线
	 * @param lTwo 双下划线
	 */
	enum Underline{lNone=-1,lOne=4,lTwo=21} underline=lNone;

	/**
	 * @brief 文本亮度枚举
	 * @param Hidden 隐藏文本
	 * @param Dark 暗色文本
	 * @param bNormal 正常亮度
	 * @param Bright 亮色文本
	 */
	enum Brightness{Hidden=8,Dark=2,bNormal=-1,Bright=1} brightness=bNormal;

	bool italic=false;    ///< 是否斜体
	bool deleted=false;   ///< 是否删除线
	bool flicker=false;   ///< 是否闪烁
	bool reversed=false;  ///< 是否反色

	/**
	 * @brief 文本颜色枚举
	 * @details 支持16种预定义颜色和RGB自定义颜色
	 */
	enum Color{
		Black=30,Red=31,Green=32,Brown=33,Blue=34,Purple=35,Teal=36,Gray=37,
		DarkGray=90,LightRed=91,LightGreen=92,Yellow=93,LightBlue=94,LightPurple=95,SkyBlue=96,White=97,cNone=-1,cRGB=38
	} color=cNone;

	/**
	 * @brief 背景颜色枚举
	 * @details 支持16种预定义颜色和RGB自定义颜色
	 */
	enum bkColor{
		bkBlack=40,bkRed=41,bkGreen=42,bkBrown=43,bkBlue=44,bkPurple=45,bkTeal=46,bkGray=47,
		bkDarkGray=100,bkLightRed=101,bkLightGreen=102,bkYellow=103,bkLightBlue=104,bkLightPurple=105,bkSkyBlue=106,bkWhite=107,bkNone=-1,bkRGB=48
	} bkcolor=bkNone;

	int r=255,g=255,b=255,bkr=0,bkg=0,bkb=0; ///< RGB颜色分量

	/**
	 * @brief 构造函数
	 * @param c 文本颜色
	 * @param bkc 背景颜色
	 * @param brightness_ 文本亮度
	 * @param flicker_ 是否闪烁
	 * @param underline_ 下划线样式
	 * @param deleted_ 是否删除线
	 * @param reversed_ 是否反色
	 * @param italic_ 是否斜体
	 */
	constexpr Pattern(Color c=cNone,bkColor bkc=bkNone,Brightness brightness_=bNormal,bool flicker_=false,Underline underline_=lNone,bool deleted_=false,bool reversed_=false,bool italic_=false)
		:underline(underline_),brightness(brightness_),italic(italic_),deleted(deleted_),flicker(flicker_),reversed(reversed_),color(c),bkcolor(bkc){}

	/**
	 * @brief 生成ANSI转义序列
	 * @return 返回格式化的ANSI转义序列字符串
	 */
	inline operator std::string()const override{
		std::string ret="\033[0";
		int temp[]={int(underline),int(brightness),(italic?3:-1),(deleted?9:-1),(flicker?5:-1),(reversed?7:-1),
		int(color),(color==cRGB?2:-1),(color==cRGB?r:-1),(color==cRGB?g:-1),(color==cRGB?b:-1),
		int(bkcolor),(bkcolor==bkRGB?2:-1),(bkcolor==bkRGB?bkr:-1),(bkcolor==bkRGB?bkg:-1),(bkcolor==bkRGB?bkb:-1)};
		for(int x:temp){
			if(x<0){
				continue;
			}
			ret+=";"+std::to_string(x);
		}
		return ret+"m";
	}

	/**
	 * @brief 样式组合
	 * @details 将当前样式与另一个样式组合，后者优先级更高
	 * @param o 另一个样式
	 * @return 返回组合后的新样式
	 */
	inline constexpr Pattern operator+(const Pattern& o)const{
		auto ret=o;
		if(ret.underline==lNone){
			ret.underline=underline;
		}
		if(ret.brightness==bNormal){
			ret.brightness=brightness;
		}
		ret.italic|=italic;
		ret.deleted|=deleted;
		ret.flicker|=flicker;
		ret.reversed|=reversed;
		if(ret.color==cNone){
			ret.color=color;
			ret.r=r;
			ret.g=g;
			ret.b=b;
		}
		if(ret.bkcolor==bkNone){
			ret.bkcolor=bkcolor;
			ret.bkr=bkr;
			ret.bkg=bkg;
			ret.bkb=bkb;
		}
		return ret;
	}

	/**
	 * @brief 链式调用
	 * @details 设置样式后输出内容，最后恢复默认样式
	 * @param out 输出流
	 * @param args 可变参数，支持多个输出项
	 * @return 返回输出流
	 */
	template<class...Args>
	inline std::ostream& operator()(std::ostream& out,Args...args)const{
		out<<*this;
		return (out<<...<<args)<<Pattern();
	}
};

/**
 * @brief 创建自定义文本颜色的 Pattern 对象
 * @param r 红色分量（0-255）
 * @param g 绿色分量（0-255）
 * @param b 蓝色分量（0-255）
 * @return 返回配置了自定义文本颜色的 Pattern 对象
 */
constexpr Pattern pRGB(u_char r,u_char g, u_char b){
	Pattern tmp(Pattern::cRGB);
	tmp.r=r;tmp.g=g;tmp.b=b;
	return tmp;
}
/**
 * @brief 创建自定义背景颜色的 Pattern 对象
 * @param r 红色分量（0-255）
 * @param g 绿色分量（0-255）
 * @param b 蓝色分量（0-255）
 * @return 返回配置了自定义背景颜色的 Pattern 对象
 */
constexpr Pattern pbkRGB(u_char r,u_char g, u_char b){
	Pattern tmp(Pattern::cNone,Pattern::bkRGB);
	tmp.bkr=r;tmp.bkg=g;tmp.bkb=b;
	return tmp;
}

/** @brief 默认样式（无特殊样式） */
constexpr Pattern pNone=Pattern();

/** @brief 黑色文本 */
constexpr Pattern pBlack=Pattern(Pattern::Black);
/** @brief 红色文本 */
constexpr Pattern pRed=Pattern(Pattern::Red);
/** @brief 绿色文本 */
constexpr Pattern pGreen=Pattern(Pattern::Green);
/** @brief 棕色文本 */
constexpr Pattern pBrown=Pattern(Pattern::Brown);
/** @brief 蓝色文本 */
constexpr Pattern pBlue=Pattern(Pattern::Blue);
/** @brief 紫色文本 */
constexpr Pattern pPurple=Pattern(Pattern::Purple);
/** @brief 青色文本 */
constexpr Pattern pTeal=Pattern(Pattern::Teal);
/** @brief 灰色文本 */
constexpr Pattern pGray=Pattern(Pattern::Gray);
/** @brief 深灰色文本 */
constexpr Pattern pDarkGray=Pattern(Pattern::DarkGray);
/** @brief 浅红色文本 */
constexpr Pattern pLightRed=Pattern(Pattern::LightRed);
/** @brief 浅绿色文本 */
constexpr Pattern pLightGreen=Pattern(Pattern::LightGreen);
/** @brief 黄色文本 */
constexpr Pattern pYellow=Pattern(Pattern::Yellow);
/** @brief 浅蓝色文本 */
constexpr Pattern pLightBlue=Pattern(Pattern::LightBlue);
/** @brief 浅紫色文本 */
constexpr Pattern pLightPurple=Pattern(Pattern::LightPurple);
/** @brief 天蓝色文本 */
constexpr Pattern pSkyBlue=Pattern(Pattern::SkyBlue);
/** @brief 白色文本 */
constexpr Pattern pWhite=Pattern(Pattern::White);

/** @brief 黑色背景 */
constexpr Pattern pbkBlack=Pattern(Pattern::cNone,Pattern::bkBlack);
/** @brief 红色背景 */
constexpr Pattern pbkRed=Pattern(Pattern::cNone,Pattern::bkRed);
/** @brief 绿色背景 */
constexpr Pattern pbkGreen=Pattern(Pattern::cNone,Pattern::bkGreen);
/** @brief 棕色背景 */
constexpr Pattern pbkBrown=Pattern(Pattern::cNone,Pattern::bkBrown);
/** @brief 蓝色背景 */
constexpr Pattern pbkBlue=Pattern(Pattern::cNone,Pattern::bkBlue);
/** @brief 紫色背景 */
constexpr Pattern pbkPurple=Pattern(Pattern::cNone,Pattern::bkPurple);
/** @brief 青色背景 */
constexpr Pattern pbkTeal=Pattern(Pattern::cNone,Pattern::bkTeal);
/** @brief 灰色背景 */
constexpr Pattern pbkGray=Pattern(Pattern::cNone,Pattern::bkGray);
/** @brief 深灰色背景 */
constexpr Pattern pbkDarkGray=Pattern(Pattern::cNone,Pattern::bkDarkGray);
/** @brief 浅红色背景 */
constexpr Pattern pbkLightRed=Pattern(Pattern::cNone,Pattern::bkLightRed);
/** @brief 浅绿色背景 */
constexpr Pattern pbkLightGreen=Pattern(Pattern::cNone,Pattern::bkLightGreen);
/** @brief 黄色背景 */
constexpr Pattern pbkYellow=Pattern(Pattern::cNone,Pattern::bkYellow);
/** @brief 浅蓝色背景 */
constexpr Pattern pbkLightBlue=Pattern(Pattern::cNone,Pattern::bkLightBlue);
/** @brief 浅紫色背景 */
constexpr Pattern pbkLightPurple=Pattern(Pattern::cNone,Pattern::bkLightPurple);
/** @brief 天蓝色背景 */
constexpr Pattern pbkSkyBlue=Pattern(Pattern::cNone,Pattern::bkSkyBlue);
/** @brief 白色背景 */
constexpr Pattern pbkWhite=Pattern(Pattern::cNone,Pattern::bkWhite);

/** @brief 隐藏文本 */
constexpr Pattern pHidden=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::Hidden);
/** @brief 暗色文本 */
constexpr Pattern pDark=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::Dark);
/** @brief 亮色文本 */
constexpr Pattern pBright=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::Bright);
/** @brief 闪烁文本 */
constexpr Pattern pFlicker=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,true);
/** @brief 单下划线文本 */
constexpr Pattern pUnderlined=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,false,Pattern::lOne);
/** @brief 双下划线文本 */
constexpr Pattern pDoubleUnderLined=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,false,Pattern::lTwo);
/** @brief 删除线文本 */
constexpr Pattern pDeleted=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,false,Pattern::lNone,true);
/** @brief 反色文本 */
constexpr Pattern pReversed=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,false,Pattern::lNone,false,true);
/** @brief 斜体文本 */
constexpr Pattern pItalic=Pattern(Pattern::cNone,Pattern::bkNone,Pattern::bNormal,false,Pattern::lNone,false,false,true);

/**
 * @example 以下是 Pattern 类的使用示例：
 * @code
 * #include <iostream>
 * #include "Char033.h"
 *
 * int main() {
 *     using namespace c033;
 *
 *     // 基本样式设置
 *     std::cout << pRed << "这是红色文本" << pNone << std::endl;
 *     std::cout << pbkGreen << "这是绿色背景" << pNone << std::endl;
 *
 *     // 组合样式
 *     Pattern style = pBlue + pUnderlined + pFlicker;
 *     std::cout << style << "这是蓝色、下划线且闪烁的文本" << pNone << std::endl;
 *
 *     // 自定义 RGB 颜色
 *     Pattern customColor = pRGB(255, 100, 50); // 橙色文本
 *     std::cout << customColor << "这是自定义 RGB 颜色的文本" << pNone << std::endl;
 *
 *     // 链式调用
 *     pRed(std::cout, "红色文本", " + ", pGreen, "绿色文本", "\n");
 *
 *     return 0;
 * }
 * @endcode
 */

}

#endif

