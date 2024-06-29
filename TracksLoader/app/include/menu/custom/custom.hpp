#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

namespace ImGui
{
	__forceinline void DrawButton( const ImGuiWindow *window, const ImVec2 &p_min, const ImVec2 &p_max, const ImU32 col_upr_left, const ImU32 col_upr_right, const ImU32 col_bot_right, const ImU32 col_bot_left, const float rounding, const ImDrawFlags rounding_corners )
	{
		if ( ( ( col_upr_left | col_upr_right | col_bot_right | col_bot_left ) & IM_COL32_A_MASK ) == 0 )
			return;

		window->DrawList->PathRect( ImVec2( p_min.x, p_min.y ), ImVec2( p_max.x, p_max.y ), rounding, rounding_corners );
		window->DrawList->PathFillConvex( col_upr_left );
		window->DrawList->PathRect( ImVec2( p_min.x + 5, p_min.y ), ImVec2( p_max.x, p_max.y ), rounding, rounding_corners );
		window->DrawList->PathFillConvex( col_bot_right );

		window->DrawList->PrimReserve( 6, 4 );
		const ImVec2 uv = window->DrawList->_Data->TexUvWhitePixel;
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx ) );
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx + 1 ) );
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx + 2 ) );
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx ) );
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx + 2 ) );
		window->DrawList->PrimWriteIdx( static_cast< ImDrawIdx >( window->DrawList->_VtxCurrentIdx + 3 ) );
		window->DrawList->PrimWriteVtx( ImVec2( p_min.x + 5, p_min.y ), uv, col_upr_left );
		window->DrawList->PrimWriteVtx( ImVec2( p_max.x - 5, p_min.y ), uv, col_upr_right );
		window->DrawList->PrimWriteVtx( ImVec2( p_max.x - 5, p_max.y ), uv, col_bot_right );
		window->DrawList->PrimWriteVtx( ImVec2( p_min.x + 5, p_max.y ), uv, col_bot_left );
	}

	__forceinline bool CustomButton( const char *label, const ImVec2 &size_arg )
	{
		ImGuiWindow *window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext &g = *GImGui;
		const ImGuiStyle &style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = ImGui::CalcTextSize( label, nullptr, true );

		const ImVec2 pos = window->DC.CursorPos;

		const ImVec2 size = ImGui::CalcItemSize( size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f );
		ImRect bb( pos, ImVec2( pos.x + size.x, pos.y + size.y ) );
		ImGui::ItemSize( size, style.FramePadding.y );
		if ( !ImGui::ItemAdd( bb, id ) )
			return false;

		static float i = 0;
		bool hovered, held;
		const bool pressed = ImGui::ButtonBehavior( bb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft );
		if ( hovered )
		{
			if ( i <= 0.7 )
			{
				i += 0.4;
			}
			bb.Expand( { 0, i } );
		}

		if ( i > 0 && !hovered )
		{
			i -= 0.4;
			bb.Expand( { 0, i } );
		}

		const ImColor button_hovered_left = ImGui::GetColorU32( ImGuiCol_Button );
		constexpr ImColor button_hovered_right = ImColor( 46, 13, 81 );

		const ImColor button_pressed_left = ImGui::GetColorU32( ImGuiCol_Button );
		constexpr ImColor button_pressed_right = ImColor( 46, 13, 81 );

		const ImColor button_left = ImGui::GetColorU32( ImGuiCol_Button );
		constexpr ImColor button_right = ImColor( 46, 13, 81 );

		DrawButton( window, ImVec2( bb.Min.x, bb.Min.y + 2 ), ImVec2( bb.Max.x, bb.Max.y + 2 ), held ? button_hovered_left : hovered ? button_pressed_left : button_left, held ? button_pressed_right : hovered ? button_hovered_right : button_right, held ? button_pressed_right : hovered ? button_hovered_right : button_right, held ? button_pressed_left : hovered ? button_hovered_left : button_left, 6, ImDrawFlags_RoundCornersAll );
		window->DrawList->AddText( ImVec2( bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize( label ).x / 2, bb.Min.y + size_arg.y / 2 + 1 - ImGui::CalcTextSize( label ).y / 2 + 2 ), ImColor( 238, 230, 236 ), label );

		return pressed;
	}

	__forceinline void ToggleButton( const char *str_id, bool *v )
	{
		ImVec2 p = ImGui::GetCursorScreenPos( );
		ImDrawList *draw_list = ImGui::GetWindowDrawList( );

		float height = 19;
		float width = height * 1.80f;
		float radius = height * 0.50f;

		ImGui::InvisibleButton( str_id, ImVec2( width, height ) );
		if ( ImGui::IsItemClicked( ) )
			*v = !*v;

		float t = *v ? 1.0f : 0.0f;

		ImGuiContext &g = *GImGui;
		float ANIM_SPEED = 0.08f;
		if ( g.LastActiveId == g.CurrentWindow->GetID( str_id ) ) // && g.LastActiveIdTimer < ANIM_SPEED)
		{
			float t_anim = ImSaturate( g.LastActiveIdTimer / ANIM_SPEED );
			t = *v ? ( t_anim ) : ( 1.0f - t_anim );
		}

		ImU32 col_bg;
		if ( ImGui::IsItemHovered( ) )
			col_bg = ImGui::GetColorU32( ImGuiCol_Border, t );
		else
			col_bg = ImGui::GetColorU32( ImGuiCol_Border, t );

		draw_list->AddRectFilled( p, ImVec2( p.x + width, p.y + height ), col_bg, height * 0.5f );
		draw_list->AddCircleFilled( ImVec2( p.x + radius + t * ( width - radius * 2.0f ), p.y + radius ), radius - 1.5f, ImGui::GetColorU32( ImGuiCol_Text ) );
	}

	inline ImVec2 operator+( const ImVec2 &lhs, const ImVec2 &rhs )
	{
		return ImVec2( lhs.x + rhs.x, lhs.y + rhs.y );
	}

	inline ImVec2 operator-( const ImVec2 &lhs, const ImVec2 &rhs )
	{
		return ImVec2( lhs.x - rhs.x, lhs.y - rhs.y );
	}

	__inline bool checkbox_c( const char *label, bool *v )
	{
		ImGuiWindow *window = GetCurrentWindow( );
		if ( window->SkipItems )
			return false;

		ImGuiContext &g = *GImGui;
		const ImGuiStyle &style = g.Style;
		const ImGuiID id = window->GetID( label );
		const ImVec2 label_size = CalcTextSize( label, nullptr, true );

		constexpr float square_sz = 13;
		const ImVec2 pos = window->DC.CursorPos;

		// Usar uma função para somar vetores ao invés do operador +
		ImVec2 total_bb_max = pos + ImVec2( square_sz + ( label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f ), square_sz );
		ImRect total_bb( pos, total_bb_max );
		ItemSize( total_bb, style.FramePadding.y );

		if ( !ItemAdd( total_bb, id ) )
		{
			IMGUI_TEST_ENGINE_ITEM_INFO( id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *v ? ImGuiItemStatusFlags_Checked : 0 ) );
			return false;
		}

		bool hovered, held;
		const bool pressed = ButtonBehavior( total_bb, id, &hovered, &held );
		if ( pressed )
		{
			*v = !( *v );
			MarkItemEdited( id );
		}

		ImVec2 check_bb_max = pos + ImVec2( square_sz, square_sz );
		ImRect check_bb( pos, check_bb_max );
		RenderNavHighlight( total_bb, id );
		RenderFrame( check_bb.Min, check_bb.Max, GetColorU32( ( held && hovered ) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg ), true, style.FrameRounding );
		const ImU32 check_col = GetColorU32( ImGuiCol_CheckMark );
		const bool mixed_value = ( g.LastItemData.InFlags & ImGuiItemFlags_MixedValue ) != 0;

		if ( mixed_value )
		{
			ImVec2 pad = ImVec2( ImMax( 1.0f, IM_FLOOR( square_sz / 3.6f ) ), ImMax( 1.0f, IM_FLOOR( square_sz / 3.6f ) ) );
			window->DrawList->AddRectFilled( check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding );
		}
		else if ( *v )
		{
			window->DrawList->AddRectFilled( check_bb.Min, check_bb.Max, check_col, style.FrameRounding );
		}

		ImVec2 label_pos = ImVec2( check_bb.Max.x + style.ItemInnerSpacing.x + 2.0f, check_bb.Min.y + style.FramePadding.y - 4.0f );
		if ( g.LogEnabled )
			LogRenderedText( &label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]" );
		if ( label_size.x > 0.0f )
			RenderText( label_pos, label );

		IMGUI_TEST_ENGINE_ITEM_INFO( id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | ( *v ? ImGuiItemStatusFlags_Checked : 0 ) );
		return pressed;
	}

	__forceinline void AddFontText( ImFont *fnt, ImVec2 pos, const char *label, ImColor col = ImGui::GetColorU32( ImGuiCol_Text ), float font_size = 13 )
	{
		ImGuiWindow *window = ImGui::GetCurrentWindow( );
		if ( window->SkipItems )
			return;

		window->DrawList->AddText( fnt, font_size, pos, col, label );
	}
}
