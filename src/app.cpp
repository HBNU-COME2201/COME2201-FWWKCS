#include <iostream>
#include <vector>
#include <string>

#include "agent.hpp"
#include "mmanager.hpp"
#include "stochastic_decorator.hpp"
#include "simple_decorator.hpp"

#include "special_agent.hpp"

#include "tinyxml2.h"



// using namespace tinyxml2; // using namespace는 지양할 것

// 추상화, 일반화를 위한 초기화 작업
class InitManager
{
public:
    InitManager(){}
    InitManager(std::string path) {
        tinyxml2::XMLDocument doc;
        doc.LoadFile(path.c_str());
        tinyxml2::XMLElement* agListElem = doc.FirstChildElement( "scenario" )->FirstChildElement( "AgentList" );

        double x, y, heading, speed, drange;
        for (tinyxml2::XMLElement* agElem = agListElem->FirstChildElement(); 
            agElem != NULL; agElem = agElem->NextSiblingElement()) // 다음 형제 요소 가져오기 
            {
                agElem->QueryDoubleAttribute("x", &x); // 요소의 x 속성 수집
                agElem->QueryDoubleAttribute("y", &y); // 요소의 y 속성 수집
                agElem->QueryDoubleAttribute("heading", &heading); // 요소의 heading 속성 수집
                agElem->QueryDoubleAttribute("speed", &speed); // 요소의 speed 속성 수집
                agElem->QueryDoubleAttribute("drange", &drange); // 요소의 drange 속성 수집

                // CAgent* ag = new CAgent(x,  y, heading, speed, drange);

                // CAgent* p_agent1 = new CAgent(x,  y, 0, 1, 10); // xml에서 쿼리를 통해 요소 가져오기
                CAgent* ag = new CSpecialAgent(x, y, 0, 1, 10); // Decorator를 위한 Agent를 상속받는 SpecialAgent 객체
                /* 다형성 인스턴스 선언 */
        
                m_agent_list.push_back(ag);
            }
    }
private:
    // 관리할 agent 인스턴스 목록
    std::vector<CAgent*> m_agent_list;

public: // 참조 반환을 통해 값의 복사(생성자, 소멸자) 제한
    std::vector<CAgent*>& get_agent_list() { return m_agent_list; }
};

int main(int argc, char** argv)
{// Clinet Code  

    InitManager init_manager("test.xml");
    /* Simulation Engine */
    CManeuverManager mmanager;

    for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        iter != init_manager.get_agent_list().end(); ++iter)
        {
            mmanager.register_publisher(*iter); // 역참조 CAgent 포인터를 얻어온다
        }


    // cleanroom approach: 코드 패턴화 작업



    // XMLElement* attributeApproachElement = doc.FirstChildElement()->FirstChildElement( "attributeApproach" );
	// attributeApproachElement->QueryIntAttribute( "v", &v0 );

    /*  Agent를 추가할때마다 새로운 생성자와 포인터 객체 얻기는 번거로움..
    우리가 제공하는 API를 통해서만 객체를 생성하세요! */

    // CAgent* p_ag = ???.getcreate_agent();
    // m_agent_list.push_back(p_ag);
    // CAgent* p_agent1 = new CAgent(10,  0, 0, 1, 10);
    // p_agent1 = new StochasticDecorator(p_agent1);
    // p_agent1 = new SimpleDecorator(p_agent1);
    
    // mmanager.register_publisher(p_agent1); 

    // CAgent* p_agent2 = new CAgent(10,  30, 180, 1, 10);
    // mmanager.register_publisher(p_agent2);
    
    

    /* Simulation Engine Initialization */
    double sim_time;
    double time_step = 1;

    for(sim_time = 0; sim_time < 30; sim_time += time_step)
    {   
        //p_agent1->maneuver(time_step);
        //p_agent2->maneuver(time_step);

        //p_agent1->detect(p_agent2);
        //p_agent2->detect(p_agent1);

        mmanager.svc(time_step);

        // std::cout << "----" << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent1 << std::endl;
        // std::cout << "Time: " << sim_time << ", " <<*p_agent2 << std::endl;
        
        std::cout << "----" << std::endl;
        for(std::vector<CAgent*>::iterator iter = init_manager.get_agent_list().begin();
        // 다형성: 하나의 객체가 여러 타입을 가질 수 있는 것
        // CSpecialAgent는 CAgent를 상속받는 상태
        // 포인터는 메모리상에서 CSpecialAgent를 가리키지만, 정의된 타입에 따라 CAgent 속성에만 접근가능하게 된다
        // CSpecialAgent가 자신만의 부가적인 속성을 가지고 있다고 해도, 부모 CAgent의 모든 속성과 기능을 가집
        // 따라서, CAgent 포인터 타입 배열을 불러와도 자식 인스턴스에 잘 접근할 수 있게 되므로 올바르게 동작할 수 있다
        iter != init_manager.get_agent_list().end(); ++iter)
        {
            std::cout << "Time: " << sim_time << ", " << *(*iter) << std::endl; 
            // 변수를 직접 정의하던 반복되는 작업이 반복자를 통해 일괄 적용됨 
        }
    }
    return 0;
}
