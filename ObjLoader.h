#ifndef Obj_Loader
#define Obj_Loader

#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include "Vehicle.h"

void loadObj(Vehicle &v, std::string filename, glm::vec3 centrum, float scale=1.0f)
{
	int offset = v.vs.getNumParticles();
	std::string tmp,chunk,group="none";
	int pos, torque_node=0, backleft=0, backright=0, frontleft=0, frontright=0;
    int steering = 0,visible=1;
	std::ifstream filestream(filename.c_str());
	std::map< std::pair<int,int>, float> springset;
	float x,y,z,w=1.0f;
    float stiffness = 1.0f;
	int fx,fy,fz,vertnum = 0;
    int t_axis = 0;
    std::set<std::pair<int,int> > leftsteerset, rightsteerset;
	std::vector< int > face, index, torque;
    std::vector< float > weight;
	std::vector< glm::vec2 > texture;
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec3 >::iterator vertices_it;
	face.resize(3);
	//texture.resize(3);
	if (filestream.is_open())
	{
        //std::cout<<"file is open"<<std::endl;
		while (filestream.good() && !filestream.eof())
		{
			filestream >> chunk;
			//read vertex coordinates x y z
			if (chunk == "v")
			{
                //std::cout<<"reading v"<<std::endl;
				filestream >> x >> y >> z;
				x = x*scale;
				y = y*scale;
				z = z*scale;
				int i = 0;
				while(i < vertnum)
				{
					if(vertices.at(i) == glm::vec3(x,y,z))
						break;
					i++;
				}
				index.push_back(i);
                
                if(i==vertnum)
                {
					vertices.push_back(glm::vec3(x,y,z));
					v.texcoord.push_back(glm::vec2(0.0,0.0));
					weight.push_back(w);
					torque.push_back(0);
                    vertnum++;
                }
				
			}
			//read faces, insert into set
			else if (chunk == "f")
			{
                //std::cout<<"reading f"<<std::endl;
				for (int i = 0; i<3; i++)
				{
					filestream >> tmp;
					//tmp = tmp.substr(0,tmp.find("/")-tmp.length());
					face[i] = atoi(tmp.c_str());
					//std::cout << tmp.substr(tmp.find("/")+1,tmp.length()) << std::endl;
					tmp = tmp.substr(tmp.find("/")+1,tmp.length());
					x = atoi(tmp.c_str());
					//only update texcoord from the following groups
					if ( group == "chassis" || group == "FL_wheel" || group == "FR_wheel" || group == "BL_wheel" || group == "BR_wheel" )
                    {
                        std::cout<<"g "<<group<<std::endl;
                        v.texcoord.at(index.at(face[i]-1)) = texture.at(x-1);
                        std::cout<<"t "<<texture[x-1].x<<" "<<texture[x-1].y<<std::endl;
                    }
				}
                
				std::sort(face.begin(),face.end());
				springset.insert( std::pair<std::pair<int,int>,float>(std::pair<int,int>( index.at(face[0]-1),index.at(face[1]-1) ), stiffness));
				springset.insert( std::pair<std::pair<int,int>,float>(std::pair<int,int>( index.at(face[1]-1),index.at(face[2]-1) ), stiffness));
				springset.insert( std::pair<std::pair<int,int>,float>(std::pair<int,int>( index.at(face[0]-1),index.at(face[2]-1) ), stiffness));
                
                if(steering==1)
                {
                    //std::cout<<"Reading left steer "<<face[0]<<" "<<face[1]<<" "<<face[2]<<std::endl;
                    leftsteerset.insert(std::pair<int,int>(index.at(face[0]-1),index.at(face[1]-1)));
                    leftsteerset.insert(std::pair<int,int>(index.at(face[1]-1),index.at(face[2]-1)));
                    leftsteerset.insert(std::pair<int,int>(index.at(face[0]-1),index.at(face[2]-1)));
                }
                if(steering==2)
                {
                    //std::cout<<"Reading right steer "<<index.at(face[0]-1)<<" "<<index.at(face[1]-1)
                    //<<" "<<index.at(face[2]-1)<<std::endl;
                    rightsteerset.insert(std::pair<int,int>(index.at(face[0]-1),index.at(face[1]-1)));
                    rightsteerset.insert(std::pair<int,int>(index.at(face[1]-1),index.at(face[2]-1)));
                    rightsteerset.insert(std::pair<int,int>(index.at(face[0]-1),index.at(face[2]-1)));
                }
				//create triangles
				if(visible==1)
				{
					if(face[0] != face[1] && face[0] != face[2] && face[1] != face[2])
					{
						v.triangles.push_back(glm::uvec3(index.at(face[0]-1),index.at(face[1]-1),index.at(face[2]-1)));
					}
				}
				
				for (int i = 0; i<3; i++)
				{
					weight.at(index.at(face[i]-1))=w;
					torque.at(index.at(face[i]-1))=torque_node;
				}
			}
			else if (chunk == "vt")
			{
				filestream >> x >> y;
				texture.push_back(glm::vec2(x,y));
				//v.texcoord.push_back(glm::vec2(x,y));
			}
			else if (chunk == "g")
				filestream >> group;
			else if (chunk == "visible")
				filestream >> visible;
			else if (chunk == "weight")
				filestream >> w;
			else if (chunk == "stiff")
				filestream >> stiffness;
			else if (chunk == "torque") //1 if back left wheel, 2 if back right, 3 front left, 4 front right
				filestream >> torque_node;
			else if (chunk == "backleft")
				filestream >> backleft;
			else if (chunk == "backright")
				filestream >> backright;
            else if (chunk == "frontleft")
                filestream >> frontleft;
            else if (chunk == "frontright")
                filestream >> frontright;
            else if (chunk == "steering")
                filestream >> steering;
            else if (chunk == "texture")
            {
                std::string s;
                filestream >> s;
                v.loadTexture(s);
            }
		}

        v.drivingwheel.push_back(offset+index.at(backleft-1));
        v.drivingwheel.push_back(offset+index.at(backright-1));
        v.drivingwheel.push_back(offset+index.at(frontleft-1));
        v.drivingwheel.push_back(offset+index.at(frontright-1));
		
        int k=0;
		for (vertices_it = vertices.begin(); vertices_it != vertices.end(); vertices_it++)
		{
            //std::cout<<"adding particle weight "<<weight.at(k)<<std::endl;
            v.vs.addParticle(centrum + *vertices_it);//, weight.at(k));
            k++;
		}
        
        v.vs.setParticleColor(offset+index.at(frontleft-1), glm::vec3(1.0,0.0,0.0) );
        v.vs.setParticleColor(offset+index.at(frontright-1), glm::vec3(0.0,0.0,1.0) );
        
        k=0;
        int leftsize, rightsize;
        leftsize = rightsize = 0;
		for (vertices_it = vertices.begin(); vertices_it != vertices.end(); vertices_it++)
		{
        	if(torque.at(k)==1)
        	{
                //std::cout<<"adding "<<offset+index.at(backleft-1)<<std::endl;
                leftsize++;
            	v.vs.addToTorqueList(offset+index.at(backleft-1), offset+k);
            	v.vs.setParticleColor(offset+k, glm::vec3(1.0,0.0,0.0) );
        	}
        	else if(torque.at(k)==2)
        	{
                //std::cout<<"adding "<<offset+index.at(backright-1)<<std::endl;
                rightsize++;
            	v.vs.addToTorqueList(offset+index.at(backright-1), offset+k);
            	v.vs.setParticleColor(offset+k, glm::vec3(0.0,0.0,1.0) );
        	}
            else if(torque.at(k)==3)
        	{
                //std::cout<<"adding "<<offset+index.at(backright-1)<<std::endl;

            	v.vs.addToTorqueList(offset+index.at(frontleft-1), offset+k);
            	v.vs.setParticleColor(offset+k, glm::vec3(1.0,0.0,0.0) );
        	}
            else if(torque.at(k)==4)
        	{
                //std::cout<<"adding "<<offset+index.at(backright-1)<<std::endl;
                
            	v.vs.addToTorqueList(offset+index.at(frontright-1), offset+k);
            	v.vs.setParticleColor(offset+k, glm::vec3(0.0,0.0,1.0) );
        	}
            k++;
        }
        
        //std::cout<<"left "<<leftsize<<" right "<<rightsize<<std::endl;
        
		//springs for each unique connection between vertices
		for (std::map< std::pair<int,int>, float >::iterator it = springset.begin(); it != springset.end(); it++)
		{
			if((*it).first.first != (*it).first.second)
			v.vs.addSpring(offset+(*it).first.first, offset+(*it).first.second,(*it).second);
			//std::cout << offset+(*it).first << " " << offset+(*it).second << std::endl;
		}
        
        //leftsteer = v.vs.findSpring(offset+index.at(213-1), offset+index.at(214-1));
        //rightsteer = v.vs.findSpring(offset+index.at(215-1), offset+index.at(216-1));
        
        //std::cout<<v.vs.findSpring(offset+index.at(185-1), offset+index.at(186-1))<<std::endl;
        //std::cout<<v.vs.findSpring(offset+index.at(183-1), offset+index.at(184-1))<<std::endl;
        
        //std::cout<<"leftsize "<<leftsteerset.size()<<std::endl;
        //std::cout<<"rightsize "<<rightsteerset.size()<<std::endl;
        
        for (std::set< std::pair<int,int> >::iterator it = leftsteerset.begin(); it != leftsteerset.end(); it++)
		{
            int spring;
            //std::cout<<"points "<<it->first<<" "<<it->second<<std::endl;
			if((*it).first != (*it).second)
            {
                spring = v.vs.findSpring(offset+(*it).first, offset+(*it).second);
                //std::cout<<"left "<<spring<<std::endl;
                
            	if(spring > -1)
                {
                    v.leftsteer.push_back(spring);
                    v.vs.setSpringColor(spring, glm::vec3(1.0,0.0,0.0) );
                }
            }
		}
        for (std::set< std::pair<int,int> >::iterator it = rightsteerset.begin(); it != rightsteerset.end(); it++)
		{
            int spring;
            //std::cout<<"points "<<it->first<<" "<<it->second<<std::endl;
			if((*it).first != (*it).second)
            {
                spring = v.vs.findSpring(offset+(*it).first, offset+(*it).second);
                //std::cout<<"right "<<spring<<std::endl;
            	if(spring > -1)
                {
                    v.rightsteer.push_back(spring);
                    v.vs.setSpringColor(spring, glm::vec3(0.0,0.0,1.0) );
                }
            }
		}
        
//        if(leftsteer >= 0)
//        {
//            v.leftsteering = leftsteer;
//            v.vs.setSpringColor(v.leftsteering, glm::vec3(1.0,1.0,0.0) );
//        }
//        if(rightsteer >= 0)
//        {
//            v.rightsteering = rightsteer;
//        	v.vs.setSpringColor(v.rightsteering, glm::vec3(0.0,1.0,1.0) );
//        }
        
        int leftwheelaxis = v.vs.findSpring(offset+index.at(602-1), offset+index.at(603-1));
		if(leftwheelaxis >= 0)
        {
            v.vs.setTorqueAxis(offset+index.at(backleft-1),leftwheelaxis);
        	v.vs.setSpringColor(leftwheelaxis, glm::vec3(1.0,0.0,0.0) );
        }
        
        //int rightwheelaxis = v.vs.findSpring(offset+index.at(151-1), offset+index.at(152-1));
        int rightwheelaxis = v.vs.findSpring(offset+index.at(604-1), offset+index.at(605-1));
		if(rightwheelaxis >= 0)
        {
            v.vs.setTorqueAxis(offset+index.at(backright-1),rightwheelaxis);
        	v.vs.setSpringColor(rightwheelaxis, glm::vec3(0.0,0.0,1.0) );
        }
        
        leftwheelaxis = v.vs.findSpring(offset+index.at(190-1), offset+index.at(599-1));
		if(leftwheelaxis >= 0)
        {
            v.vs.setTorqueAxis(offset+index.at(frontleft-1),leftwheelaxis);
        	v.vs.setSpringColor(leftwheelaxis, glm::vec3(1.0,0.0,0.0) );
        }
        
        rightwheelaxis = v.vs.findSpring(offset+index.at(601-1), offset+index.at(306-1));
        if(rightwheelaxis >= 0)
        {
            v.vs.setTorqueAxis(offset+index.at(frontright-1),rightwheelaxis);
        	v.vs.setSpringColor(rightwheelaxis, glm::vec3(0.0,0.0,1.0) );
        }
        
        v.vs.setParticleColor(offset+index.at(backleft-1), glm::vec3(1.0,0.0,0.0) );
        v.vs.setParticleColor(offset+index.at(backright-1), glm::vec3(0.0,0.0,1.0) );
        
		filestream.close();
	}
}
#endif
