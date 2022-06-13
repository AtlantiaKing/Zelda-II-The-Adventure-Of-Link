#include "pch.h"
#include <iostream>
#include "SoundEffect.h"

SoundEffect::SoundEffect( const std::string& path )
	:m_pMixChunk{ Mix_LoadWAV( path.c_str( ) ) }
{
	if ( m_pMixChunk == nullptr )
	{
		std::string errorMsg = "SoundEffect: Failed to load " + path + ",\nSDL_mixer Error: " + Mix_GetError( );
		std::cerr << errorMsg;
	}
}
SoundEffect::~SoundEffect( )
{
	Mix_FreeChunk( m_pMixChunk );
	m_pMixChunk = nullptr;
}

bool SoundEffect::IsLoaded( ) const
{
	return m_pMixChunk != nullptr;
}

std::pair<bool,int> SoundEffect::Play( int loops ) const
{
	if ( m_pMixChunk != nullptr )
	{
		int channel{ Mix_PlayChannel( -1, m_pMixChunk, loops ) };
		return std::make_pair(channel != -1, channel);
	}
	else
	{
		return std::make_pair(false, -1);
	}
}

void SoundEffect::SetVolume( int value )
{
	if ( m_pMixChunk != nullptr )
	{
		Mix_VolumeChunk( m_pMixChunk, value );
	}
}

int SoundEffect::GetVolume( ) const
{
	if ( m_pMixChunk != nullptr )
	{
		return Mix_VolumeChunk( m_pMixChunk, -1 );
	}
	else
	{
		return -1;
	}
}

void SoundEffect::StopAll( )
{
	Mix_HaltChannel(-1 );
}

void SoundEffect::PauseAll( )
{
	Mix_Pause( -1 );
}
void SoundEffect::ResumeAll( )
{
	Mix_Resume( -1 );
}



