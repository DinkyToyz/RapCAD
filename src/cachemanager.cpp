#include "cachemanager.h"

CacheManager* CacheManager::instance=NULL;

CacheManager* CacheManager::getInstance()
{
	if(!instance)
		instance=new CacheManager();
	return instance;
}

Cache* CacheManager::getCache() const
{
	return cache;
}

void CacheManager::setCache(Cache* c)
{
	cache=c;
}

CacheManager::CacheManager()
{
	cache=NULL;
}
